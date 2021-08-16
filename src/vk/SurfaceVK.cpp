#include "SurfaceVK.h"
#include "DeviceVK.h"
#include "SemaphoreVK.h"
#include "VulkanConvert.h"
#include "VulkanHelpers.h"

namespace kuas
{
    SurfaceVK::SurfaceVK(
        VkSurfaceKHR surface,
        VkSwapchainKHR swapchain,
        ImageUsageFlags usage,
        PixelFormat pixFmt,
        VkColorSpaceKHR colorSpace,
        VkCommandPool cmdPool,
        DeviceVK* parentDevice) :
        m_surface(surface),
        m_swapchain(VK_NULL_HANDLE),
        m_usage(usage),
        m_pixFmt(pixFmt),
        m_colorSpace(colorSpace),
        m_cmdPool(cmdPool),
        m_width(0),
        m_height(0),
        m_currentBackBuffer(0),
        m_fn(parentDevice->getFunc()),
        m_parentDevice(parentDevice)
    {
        init(swapchain);
    }
    
    SurfaceVK::~SurfaceVK()
    {
        VkDevice device = m_parentDevice->getDevice();

        m_parentDevice->waitIdle();

        for (uint32_t i = 0; i < m_numImages; i++) {
            m_images[i]->release();
        }

        destroySwapchainObjects();

        m_fn.vkDestroyCommandPool(device, m_cmdPool, nullptr);
        m_fn.vkDestroySwapchainKHR(device, m_swapchain, nullptr);
        m_fn.vkDestroySurfaceKHR(m_parentDevice->getInstance(), m_surface, nullptr);
    }
    
    Result SurfaceVK::resize(uint32_t width, uint32_t height)
    {
        VkDevice device = m_parentDevice->getDevice();
        VmaAllocator allocator = m_parentDevice->getAllocator();
        
        m_parentDevice->waitIdle();

        VkImageCreateInfo imgInfo{};
        imgInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imgInfo.imageType = VK_IMAGE_TYPE_2D;
        imgInfo.format = convertPixelFormatVk(m_pixFmt);
        imgInfo.extent.width = width;
        imgInfo.extent.height = height;
        imgInfo.extent.depth = 1;
        imgInfo.mipLevels = 1;
        imgInfo.arrayLayers = 1;
        imgInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imgInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imgInfo.usage = convertImageUsageVk(m_usage) | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        imgInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        ImageCreateDesc imgDesc{};
        imgDesc.format = m_pixFmt;
        imgDesc.width = width;
        imgDesc.height = height;
        imgDesc.usage = m_usage | ImageUsage::TransferSrc;

        VkImageViewCreateInfo imgView{};
        imgView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imgView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imgView.format = convertPixelFormatVk(m_pixFmt);
        imgView.components.r = VK_COMPONENT_SWIZZLE_R;
        imgView.components.g = VK_COMPONENT_SWIZZLE_G;
        imgView.components.b = VK_COMPONENT_SWIZZLE_B;
        imgView.components.a = VK_COMPONENT_SWIZZLE_A;
        imgView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imgView.subresourceRange.baseMipLevel = 0;
        imgView.subresourceRange.levelCount = 1;
        imgView.subresourceRange.baseArrayLayer = 0;
        imgView.subresourceRange.layerCount = 1;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        for (uint32_t i = 0; i < m_numImages; i++) {
            VkImage image;
            VkImageView imageView;
            VmaAllocation allocation = VK_NULL_HANDLE;

            // We don't use createImage() because we need to resize the image memory when needed and avoid memory reallocation.
            VkResult result = m_fn.vkCreateImage(device, &imgInfo, nullptr, &image);
            KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

            if (m_images[i] != nullptr) {
                allocation = m_images[i]->m_allocation;
            }

            if (width > m_width || height > m_height) {
                VmaAllocation newAllocation;
                result = vmaAllocateMemoryForImage(allocator, image, &allocInfo, &newAllocation, nullptr);
                KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

                if (allocation != VK_NULL_HANDLE) {
                    vmaFreeMemory(allocator, allocation);
                }
                
                allocation = newAllocation;
            }

            vmaBindImageMemory(allocator, allocation, image);

            imgView.image = image;
            result = m_fn.vkCreateImageView(device, &imgView, nullptr, &imageView);
            KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

            if (m_images[i] == nullptr) {
                m_images[i] = new ImageVK(imgDesc, image, imageView, allocation, m_parentDevice);
            }
            else {
                m_fn.vkDestroyImage(device, m_images[i]->m_image, nullptr);
                m_fn.vkDestroyImageView(device, m_images[i]->m_imageView, nullptr);

                m_images[i]->m_allocation = allocation;
                m_images[i]->m_image = image;
                m_images[i]->m_imageView = imageView;
                m_images[i]->m_width = width;
                m_images[i]->m_height = height;
            }
        }

        m_width = width;
        m_height = height;

        return Result::Ok;
    }

    Result SurfaceVK::swapBuffer(uint32_t numWaitSemaphores, Semaphore* const* semaphores)
    {
        VkDevice device = m_parentDevice->getDevice();
        VkQueue queue = m_parentDevice->getQueue();
        VkCommandBuffer cb = m_cmdBuffers[m_currentBackBuffer];
        VkSemaphore* waitSemaphores = (VkSemaphore*)alloca(sizeof(VkSemaphore) * (size_t)(numWaitSemaphores + 1)); // reserve one for image acquiring semaphore
        VkPipelineStageFlags* waitStageDst = (VkPipelineStageFlags*)alloca(sizeof(VkPipelineStageFlags) * (size_t)(numWaitSemaphores + 1));
        VkSemaphore imageAcquired = m_imageAcquiredSemaphores[m_currentBackBuffer];
        VkSemaphore queueFinished = m_queueFinishedSemaphores[m_currentBackBuffer];

        VkResult result = m_fn.vkAcquireNextImageKHR(
            device, m_swapchain, UINT64_MAX,
            imageAcquired, nullptr, &m_currentBackBuffer);

        if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR) {
            m_parentDevice->waitIdle();
            init(nullptr);
            m_currentBackBuffer = 0;
            imageAcquired = m_imageAcquiredSemaphores[m_currentBackBuffer];
            queueFinished = m_queueFinishedSemaphores[m_currentBackBuffer];
            result = m_fn.vkAcquireNextImageKHR(
                device, m_swapchain, UINT64_MAX,
                imageAcquired, nullptr, &m_currentBackBuffer);
        }

        for (uint32_t i = 0; i < numWaitSemaphores; i++) {
            waitSemaphores[i] = KUAS_PTR_CAST(SemaphoreVK, semaphores[i])->m_semaphore;
        }

        if (numWaitSemaphores != 0) {
            std::fill_n(waitStageDst, numWaitSemaphores, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
        }

        waitSemaphores[numWaitSemaphores] = imageAcquired;
        waitStageDst[numWaitSemaphores] = VK_PIPELINE_STAGE_TRANSFER_BIT;

        VkFence frameFence = m_frameFences[m_currentBackBuffer];
        VkFence prevFrameFence = m_frameFences[(m_currentBackBuffer - 1) % m_numImages];
        m_fn.vkWaitForFences(device, 1, &prevFrameFence, VK_TRUE, UINT64_MAX);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        m_fn.vkBeginCommandBuffer(cb, &beginInfo);
        {
            VkImage srcImage = m_images[m_currentBackBuffer]->m_image;
            VkImage dstImage = m_surfaceImages[m_currentBackBuffer];

            vulkanImageMemoryBarrier(
                m_fn, cb, srcImage,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

            vulkanImageMemoryBarrier(
                m_fn, cb, dstImage,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                0,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VkImageBlit blitRegion{};
            blitRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blitRegion.srcSubresource.mipLevel = 0;
            blitRegion.srcSubresource.baseArrayLayer = 0;
            blitRegion.srcSubresource.layerCount = 1;
            blitRegion.srcOffsets[1].x = m_width;
            blitRegion.srcOffsets[1].y = m_height;
            blitRegion.srcOffsets[1].z = 1;
            blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blitRegion.dstSubresource.mipLevel = 0;
            blitRegion.dstSubresource.baseArrayLayer = 0;
            blitRegion.dstSubresource.layerCount = 1;
            blitRegion.dstOffsets[1].x = m_surfaceWidth;
            blitRegion.dstOffsets[1].y = m_surfaceHeight;
            blitRegion.dstOffsets[1].z = 1;

            // Blit "facade" image to the surface image
            m_fn.vkCmdBlitImage(
                cb, srcImage, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                dstImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blitRegion, VK_FILTER_LINEAR);

            vulkanImageMemoryBarrier(
                m_fn, cb, dstImage,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                VK_ACCESS_TRANSFER_WRITE_BIT,
                0,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

            vulkanImageMemoryBarrier(
                m_fn, cb, srcImage,
                VK_PIPELINE_STAGE_TRANSFER_BIT,
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                VK_ACCESS_TRANSFER_READ_BIT,
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
                VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        }
        m_fn.vkEndCommandBuffer(cb);

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount = numWaitSemaphores + 1;
        submit.pWaitSemaphores = waitSemaphores;
        submit.pWaitDstStageMask = waitStageDst;
        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &cb;
        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &queueFinished;

        m_fn.vkResetFences(device, 1, &frameFence);
        m_fn.vkQueueSubmit(queue, 1, &submit, frameFence);

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &queueFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapchain;
        presentInfo.pImageIndices = &m_currentBackBuffer;

        m_fn.vkQueuePresentKHR(queue, &presentInfo);

        m_currentBackBuffer = (m_currentBackBuffer + 1) % m_numImages;
        return Result::Ok;
    }

    uint32_t SurfaceVK::getCurrentBackBuffer() const
    {
        return m_currentBackBuffer;
    }

    uint32_t SurfaceVK::getNumImages() const
    {
        return m_numImages;
    }

    Image* SurfaceVK::getImage(uint32_t n)
    {
        return m_images[n];
    }
    
    void SurfaceVK::init(VkSwapchainKHR swapchain)
    {
        VkDevice device = m_parentDevice->getDevice();
        VkSurfaceCapabilitiesKHR surfCaps{};

        m_fn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_parentDevice->getPhysicalDevice(), m_surface, &surfCaps);
        m_swapchain = swapchain;

        if (swapchain == nullptr) {
            destroySwapchainObjects();

            VkSwapchainKHR newSwapchain;
            VkSwapchainCreateInfoKHR swapchainInfo{};
            swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            swapchainInfo.surface = m_surface;
            swapchainInfo.minImageCount = std::min(std::max(surfCaps.minImageCount, m_numImages), surfCaps.maxImageCount);
            swapchainInfo.imageFormat = convertPixelFormatVk(m_pixFmt);
            swapchainInfo.imageColorSpace = m_colorSpace;
            swapchainInfo.imageExtent.width = surfCaps.maxImageExtent.width;
            swapchainInfo.imageExtent.height = surfCaps.maxImageExtent.height;
            swapchainInfo.imageArrayLayers = 1;
            swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
            swapchainInfo.clipped = VK_TRUE;
            swapchainInfo.oldSwapchain = m_swapchain;

            VkResult result = m_fn.vkCreateSwapchainKHR(device, &swapchainInfo, nullptr, &newSwapchain);
            KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

            m_fn.vkDestroySwapchainKHR(device, m_swapchain, nullptr);
            m_swapchain = newSwapchain;
        }

        VkResult result = m_fn.vkGetSwapchainImagesKHR(device, m_swapchain, &m_numImages, nullptr);
        KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

        result = m_fn.vkGetSwapchainImagesKHR(device, m_swapchain, &m_numImages, m_surfaceImages.data());
        KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        if (m_width == 0 && m_height == 0) {
            resize(surfCaps.maxImageExtent.width, surfCaps.maxImageExtent.height);

            VkCommandBufferAllocateInfo cbInfo{};
            cbInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cbInfo.commandPool = m_cmdPool;
            cbInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cbInfo.commandBufferCount = m_numImages;

            result = m_fn.vkAllocateCommandBuffers(device, &cbInfo, m_cmdBuffers.data());
            KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));
        }

        m_surfaceWidth = surfCaps.maxImageExtent.width;
        m_surfaceHeight = surfCaps.maxImageExtent.height;

        for (uint32_t i = 0; i < m_numImages; i++) {
            result = m_fn.vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_imageAcquiredSemaphores[i]);
            KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

            result = m_fn.vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_queueFinishedSemaphores[i]);
            KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));

            result = m_fn.vkCreateFence(device, &fenceInfo, nullptr, &m_frameFences[i]);
            KUAS_ASSERT(!KUAS_VULKAN_FAILED(result));
        }
    }

    void SurfaceVK::destroySwapchainObjects()
    {
        VkDevice device = m_parentDevice->getDevice();

        for (uint32_t i = 0; i < m_numImages; i++) {
            m_fn.vkDestroySemaphore(device, m_imageAcquiredSemaphores[i], nullptr);
            m_fn.vkDestroySemaphore(device, m_queueFinishedSemaphores[i], nullptr);
            m_fn.vkDestroyFence(device, m_frameFences[i], nullptr);
        }
    }
}
