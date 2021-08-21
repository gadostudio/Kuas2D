#include "DeviceVK.h"
#include "VulkanConvert.h"
#include <kuas/DebugAssert.h>

#include "DrawPassVK.h"
#include "RenderTargetVK.h"
#include "RenderStateVK.h"
#include "DrawListVK.h"
#include "FenceVK.h"
#include "SemaphoreVK.h"
#include "DrawParamVK.h"
#include "PipelineBuilderVK.h"

#include <algorithm>

#ifdef KUAS_USE_SDL
#include <SDL.h>
#include <SDL_vulkan.h>
#endif

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26812)
#pragma warning(disable : 6255)
#endif // _MSC_VER

namespace kuas
{
    DeviceVK::DeviceVK(const VulkanDeviceCreateDesc& desc) :
        m_instance(desc.instance),
        m_physicalDevice(desc.physicalDevice),
        m_device(desc.device),
        m_graphicsQueue(desc.graphicsQueue),
        m_graphicsQueueFamily(desc.graphicsQueueFamilyIndex),
        m_shaderModules{0},
        m_fn()
    {
        if (desc.instanceProcAddrFn != nullptr && desc.deviceProcAddrFn != nullptr) {
            initFunctionsAuto(desc.instanceProcAddrFn, desc.deviceProcAddrFn);
        }
        else {
            initFunctions(*desc.functions);
        }

        if (m_graphicsQueue == nullptr) {
            m_fn.vkGetDeviceQueue(m_device, m_graphicsQueueFamily, 0, &m_graphicsQueue);
        }

        initVma();
        initShaderModule();

        VkPipelineCacheCreateInfo cacheInfo{};
        cacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreatePipelineCache(m_device, &cacheInfo, nullptr, &m_pipelineCache))) {
            KUAS_ASSERT(false && "Failed to create vulkan pipeline cache");
        }

        VkPushConstantRange pushConstant{};
        pushConstant.offset = 0;
        pushConstant.size = sizeof(DrawParamVK);
        pushConstant.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;

        VkPipelineLayoutCreateInfo layoutInfo{};
        layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        layoutInfo.pushConstantRangeCount = 1;
        layoutInfo.pPushConstantRanges = &pushConstant;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreatePipelineLayout(m_device, &layoutInfo, nullptr, &m_commonPipelineLayout))) {
            KUAS_ASSERT(false && "Failed to create vulkan pipeline cache");
        }
    }

    DeviceVK::~DeviceVK()
    {
        m_fn.vkDeviceWaitIdle(m_device);

        for (auto sm : m_shaderModules) {
            if (sm != nullptr) {
                m_fn.vkDestroyShaderModule(m_device, sm, nullptr);
            }
        }

        m_fn.vkDestroyPipelineLayout(m_device, m_commonPipelineLayout, nullptr);
        m_fn.vkDestroyPipelineCache(m_device, m_pipelineCache, nullptr);
        vmaDestroyAllocator(m_allocator);
    }

    Result DeviceVK::createBitmap(const BitmapCreateDesc& desc, Bitmap** bitmap)
    {
        return Result();
    }
    
    Result DeviceVK::createImage(const ImageCreateDesc& desc, Image** image)
    {
        if (desc.usage == 0) {
            return Result::ErrInvalidArg;
        }

        VkImage vkimage;
        VmaAllocation allocation;

        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = convertPixelFormatVk(desc.format);
        imageInfo.extent.width = desc.width;
        imageInfo.extent.height = desc.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = convertImageUsageVk(desc.usage);
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.preferredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        if (KUAS_VULKAN_FAILED(vmaCreateImage(m_allocator, &imageInfo, &allocInfo, &vkimage, &allocation, nullptr))) {
            return Result::ErrOutOfMemory;
        }

        static constexpr ImageUsageFlags viewRequiredFlags = 0
            | ImageUsage::Drawing
            | ImageUsage::RenderTargetOutput
            | ImageUsage::RenderTargetInput;

        VkImageView view = nullptr;
        
        if ((desc.usage & viewRequiredFlags) != 0) {
            VkImageViewCreateInfo viewInfo{};

            viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            viewInfo.image = vkimage;
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            viewInfo.format = imageInfo.format;
            viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            viewInfo.subresourceRange.baseMipLevel = 0;
            viewInfo.subresourceRange.levelCount = 1;
            viewInfo.subresourceRange.baseArrayLayer = 0;
            viewInfo.subresourceRange.layerCount = 1;

            if (KUAS_VULKAN_FAILED(m_fn.vkCreateImageView(m_device, &viewInfo, nullptr, &view))) {
                return Result::ErrOutOfMemory;
            }
        }

        *image = new ImageVK(desc, vkimage, view, allocation, this);

        return Result::Ok;
    }

    Result DeviceVK::createDrawPass(const DrawPassCreateDesc& desc, DrawPass** drawPass)
    {
        VkRenderPass renderPass;
        VkRenderPassCreateInfo rpInfo{};
        VkAttachmentDescription attDesc{};
        VkAttachmentReference attRef{};
        VkSubpassDescription subpassDesc{};

        attDesc.format = convertPixelFormatVk(desc.format);
        attDesc.samples = VK_SAMPLE_COUNT_1_BIT;
        attDesc.loadOp = convertLoadOpVk(desc.loadOp);
        attDesc.storeOp = convertStoreOpVk(desc.storeOp);
        attDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attDesc.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        if (attDesc.loadOp == VK_ATTACHMENT_LOAD_OP_MAX_ENUM
            || attDesc.storeOp == VK_ATTACHMENT_STORE_OP_MAX_ENUM)
        {
            return Result::ErrInvalidArg;
        }

        attRef.attachment = 0;
        attRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        subpassDesc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDesc.colorAttachmentCount = 1;
        subpassDesc.pColorAttachments = &attRef;

        rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        rpInfo.attachmentCount = 1;
        rpInfo.pAttachments = &attDesc;
        rpInfo.subpassCount = 1;
        rpInfo.pSubpasses = &subpassDesc;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreateRenderPass(m_device, &rpInfo, nullptr, &renderPass))) {
            return Result::ErrOutOfMemory;
        }

        *drawPass = new DrawPassVK(renderPass, this);

        return Result::Ok;
    }

    Result DeviceVK::createRenderState(DrawPass* drawPass, const RenderStateCreateDesc& desc, RenderState** renderState)
    {
        static const VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_BLEND_CONSTANTS,
        };

        PipelineBuilderVK builder(
            desc, KUAS_PTR_CAST(DrawPassVK, drawPass)->getHandle(), m_pipelineCache, this);

        VkPipeline rect = builder.buildRect();
        KUAS_ASSERT(rect != nullptr && "Failed to create graphics pipeline");
        
        VkPipeline roundedRect = builder.buildRoundedRect();
        KUAS_ASSERT(roundedRect != nullptr && "Failed to create graphics pipeline");

        VkPipeline fillRect = builder.buildFillRect();
        KUAS_ASSERT(fillRect != nullptr && "Failed to create graphics pipeline");

        VkPipeline fillRoundedRect = builder.buildFillRoundedRect();
        KUAS_ASSERT(fillRoundedRect != nullptr && "Failed to create graphics pipeline");

        VkPipeline circle = builder.buildCircle();
        KUAS_ASSERT(circle != nullptr && "Failed to create graphics pipeline");

        VkPipeline fillCircle = builder.buildFillCircle();
        KUAS_ASSERT(fillCircle != nullptr && "Failed to create graphics pipeline");

        VkPipeline fillEllipse = builder.buildFillEllipse();
        KUAS_ASSERT(fillEllipse != nullptr && "Failed to create graphics pipeline");

        *renderState = new RenderStateVK(
            rect,
            roundedRect,
            circle,
            fillRect,
            fillRoundedRect,
            fillCircle,
            fillEllipse,
            ColorStateDesc{},
            this);

        return Result::Ok;
    }
    
    Result DeviceVK::createRenderTarget(
        DrawPass* drawPass,
        Image* imageBinding,
        uint32_t width,
        uint32_t height,
        RenderTarget** renderTarget)
    {
        VkFramebuffer fb;
        VkFramebufferCreateInfo fbInfo{};
        ImageVK* imgVk = KUAS_PTR_CAST(ImageVK, imageBinding);
        DrawPassVK* drawPassVk = KUAS_PTR_CAST(DrawPassVK, drawPass);
        VkImageView imgView = imgVk->getImageView();

        fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbInfo.renderPass = drawPassVk->getHandle();
        fbInfo.attachmentCount = 1;
        fbInfo.pAttachments = &imgView;
        fbInfo.width = width;
        fbInfo.height = height;
        fbInfo.layers = 1;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreateFramebuffer(m_device, &fbInfo, nullptr, &fb))) {
            return Result::ErrOutOfMemory;
        }

        *renderTarget = new RenderTargetVK(fb, width, height, this);

        return Result::Ok;
    }

    Result DeviceVK::createPathBuffer()
    {
        return Result();
    }

    Result DeviceVK::createDrawList(DrawList** drawList)
    {
        VkCommandPool cmdPool;
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_graphicsQueueFamily;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreateCommandPool(m_device, &poolInfo, nullptr, &cmdPool))) {
            return Result::ErrOutOfMemory;
        }

        *drawList = new DrawListVK(cmdPool, this);

        return Result::Ok;
    }

    Result DeviceVK::createSemaphore(Semaphore** semaphore)
    {
        VkSemaphore vksemaphore;
        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &vksemaphore))) {
            return Result::ErrOutOfMemory;
        }

        *semaphore = new SemaphoreVK(vksemaphore, this);

        return Result::Ok;
    }

    Result DeviceVK::createFence(bool signaled, Fence** fence)
    {
        VkFence vkfence;
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreateFence(m_device, &fenceInfo, nullptr, &vkfence))) {
            return Result::ErrOutOfMemory;
        }

        *fence = new FenceVK(vkfence, this);

        return Result::Ok;
    }

    Result DeviceVK::createSurface(const SurfaceCreateDesc& desc, Surface** surface)
    {
        VkSurfaceKHR vksurface;
        bool ret;

        if (desc.numSwapBuffer > 4) {
            return Result::ErrInvalidArg;
        }

        switch (desc.type) {
#ifdef KUAS_USE_SDL
            case SurfaceType::SDL:
                ret = createSurfaceSDL(desc, &vksurface);
                break;
#endif
#ifdef KUAS_PLATFORM_WINDOWS
            case SurfaceType::Win32:
                ret = createSurfaceWin32(desc, &vksurface);
                break;
#endif
            default:
                return Result::ErrInvalidArg;
        }

        if (!ret) {
            return Result::ErrUnknown;
        }

        VkBool32 surfaceSupport;
        m_fn.vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, m_graphicsQueueFamily, vksurface, &surfaceSupport);
        KUAS_ASSERT(surfaceSupport == VK_TRUE);

        VkSurfaceCapabilitiesKHR surfCaps;
        m_fn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, vksurface, &surfCaps);

        uint32_t numFormats;
        std::vector<VkSurfaceFormatKHR> supportedFormats;

        m_fn.vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, vksurface, &numFormats, nullptr);
        supportedFormats.resize(numFormats);
        m_fn.vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, vksurface, &numFormats, supportedFormats.data());

        PixelFormat requestedFmt = desc.format;
        VkFormat nativeFormat = convertPixelFormatVk(requestedFmt);
        VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;
        bool fmtSupported = false;

        for (const auto& fmt : supportedFormats) {
            if (fmt.format == nativeFormat) {
                colorSpace = fmt.colorSpace;
                fmtSupported = true;
                break;
            }
        }

        if (!fmtSupported) {
            nativeFormat = supportedFormats[0].format;
            colorSpace = supportedFormats[0].colorSpace;
        }

        VkSwapchainKHR swapchain;
        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = vksurface;
        swapchainInfo.minImageCount = std::min(std::max(surfCaps.minImageCount, desc.numSwapBuffer), surfCaps.maxImageCount);
        swapchainInfo.imageFormat = nativeFormat;
        swapchainInfo.imageColorSpace = colorSpace;
        swapchainInfo.imageExtent.width = surfCaps.maxImageExtent.width;
        swapchainInfo.imageExtent.height = surfCaps.maxImageExtent.height;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreateSwapchainKHR(m_device, &swapchainInfo, nullptr, &swapchain))) {
            m_fn.vkDestroySurfaceKHR(m_instance, vksurface, nullptr);
            return Result::ErrOutOfMemory;
        }

        VkCommandPool cmdPool;
        VkCommandPoolCreateInfo cmdPoolInfo{};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolInfo.queueFamilyIndex = m_graphicsQueueFamily;

        if (KUAS_VULKAN_FAILED(m_fn.vkCreateCommandPool(m_device, &cmdPoolInfo, nullptr, &cmdPool))) {
            m_fn.vkDestroySwapchainKHR(m_device, swapchain, nullptr);
            m_fn.vkDestroySurfaceKHR(m_instance, vksurface, nullptr);
            return Result::ErrOutOfMemory;
        }

        *surface = new SurfaceVK(vksurface, swapchain, desc.usage, requestedFmt, colorSpace, cmdPool, this);

        return Result::Ok;
    }

    void DeviceVK::mapBitmap()
    {
    }

    void DeviceVK::unmapBitmap()
    {
    }

    Result DeviceVK::enqueueWait(uint32_t numWaitSemaphores, Semaphore* const* waitSemaphores)
    {
        VkPipelineStageFlags* waitStageDest = (VkPipelineStageFlags*)alloca(sizeof(VkPipelineStageFlags) * 64);

        for (uint32_t i = 0; i < numWaitSemaphores; i++) {
            if (waitSemaphores[i] == nullptr) {
                return Result::ErrInvalidArg;
            }

            VkSemaphore semaphore = KUAS_PTR_CAST(SemaphoreVK, waitSemaphores[i])->m_semaphore;
            
            // flush wait semaphores when the queue is full
            if (!m_waitQueue.push(semaphore)) {
                KUAS_ASSERT(waitSemaphores[i] != nullptr);
                VkSemaphore* queued = m_waitQueue.getData();

                std::fill_n(waitStageDest, 64, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

                VkSubmitInfo submit{};
                submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit.waitSemaphoreCount = 64;
                submit.pWaitSemaphores = queued;
                submit.pWaitDstStageMask = waitStageDest;

                m_fn.vkQueueSubmit(m_graphicsQueue, 1, &submit, nullptr);

                m_waitQueue.flush();
                m_waitQueue.push(semaphore);
            }
        }

        return Result::Ok;
    }

    Result DeviceVK::enqueueDrawLists(
        uint32_t numDrawLists,
        DrawList* const* drawLists)
    {
        for (uint32_t i = 0; i < numDrawLists; i++) {
            if (drawLists[i] == nullptr) {
                return Result::ErrInvalidArg;
            }

            VkCommandBuffer cmdBuffer = KUAS_PTR_CAST(DrawListVK, drawLists[i])->m_cmdBuffer;
            
            // flush command buffers when the queue is full
            if (!m_cmdBufferQueue.push(cmdBuffer)) {
                VkCommandBuffer* queued = m_cmdBufferQueue.getData();

                VkSubmitInfo submit{};
                submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submit.commandBufferCount = 64;
                submit.pCommandBuffers = queued;

                m_fn.vkQueueSubmit(m_graphicsQueue, 1, &submit, nullptr);

                m_cmdBufferQueue.flush();
                m_cmdBufferQueue.push(cmdBuffer);
            }
        }

        return Result::Ok;
    }

    Result DeviceVK::enqueueSignal(uint32_t numSignalSemaphores, Semaphore* const* signalSemaphores, Fence* signalFence)
    {
        VkSemaphore* semaphores = (VkSemaphore*)alloca(sizeof(VkSemaphore) * numSignalSemaphores);
        VkFence fence = nullptr;

        if (signalFence != nullptr) {
            fence = KUAS_PTR_CAST(FenceVK, signalFence)->m_fence;
        }

        for (uint32_t i = 0; i < numSignalSemaphores; i++) {
            semaphores[i] = KUAS_PTR_CAST(SemaphoreVK, signalSemaphores[i])->m_semaphore;
        }

        uint32_t numWaitSemaphores = (uint32_t)m_waitQueue.getWritePos();
        uint32_t numCmdBuffers = (uint32_t)m_cmdBufferQueue.getWritePos();
        VkPipelineStageFlags* waitStageDest = (VkPipelineStageFlags*)alloca(sizeof(VkPipelineStageFlags) * numWaitSemaphores);

        std::fill_n(waitStageDest, numWaitSemaphores, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

        VkSubmitInfo submit{};
        submit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit.waitSemaphoreCount = numWaitSemaphores;
        submit.pWaitSemaphores = m_waitQueue.getData();
        submit.pWaitDstStageMask = waitStageDest;
        submit.commandBufferCount = numCmdBuffers;
        submit.pCommandBuffers = m_cmdBufferQueue.getData();
        submit.signalSemaphoreCount = numSignalSemaphores;
        submit.pSignalSemaphores = semaphores;
        
        m_waitQueue.flush();
        m_cmdBufferQueue.flush();
        m_fn.vkQueueSubmit(m_graphicsQueue, 1, &submit, fence);

        return Result::Ok;
    }

    Result DeviceVK::waitIdle()
    {
        VkResult result = m_fn.vkDeviceWaitIdle(m_device);

        switch (result) {
            case VK_ERROR_DEVICE_LOST:
                return Result::ErrDeviceLost;
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            case VK_ERROR_OUT_OF_HOST_MEMORY:
                return Result::ErrOutOfMemory;
        }

        return Result::Ok;
    }

    Result DeviceVK::checkBitmapPixelFormatSupport(PixelFormat format, BitmapUsageFlags usage)
    {
        return Result::Ok;
    }

    Result DeviceVK::checkImagePixelFormatSupport(PixelFormat format, ImageUsageFlags usage)
    {
        VkImageFormatProperties prop{};

        VkResult result = m_fn.vkGetPhysicalDeviceImageFormatProperties(
            m_physicalDevice,
            convertPixelFormatVk(format),
            VK_IMAGE_TYPE_2D,
            VK_IMAGE_TILING_OPTIMAL,
            convertImageUsageVk(usage),
            0,
            &prop);

        switch (result) {
            case VK_ERROR_FORMAT_NOT_SUPPORTED:
                return Result::ErrFormatNotSupported;
            case VK_ERROR_OUT_OF_HOST_MEMORY:
            case VK_ERROR_OUT_OF_DEVICE_MEMORY:
                return Result::ErrUnknown;
        }

        (void)prop;

        return Result::Ok;
    }

    void DeviceVK::destroyImage(ImageVK* image)
    {
        if (image->m_imageView != nullptr) {
            m_fn.vkDestroyImageView(m_device, image->m_imageView, nullptr);
        }

        vmaDestroyImage(m_allocator, image->m_image, image->m_allocation);
    }

    void DeviceVK::initFunctionsAuto(PFN_vkGetInstanceProcAddr instanceLoadFn, PFN_vkGetDeviceProcAddr loadFn)
    {
        // instance functions
        m_fn.vkGetPhysicalDeviceProperties = (PFN_vkGetPhysicalDeviceProperties)instanceLoadFn(m_instance, "vkGetPhysicalDeviceProperties");
        m_fn.vkGetPhysicalDeviceMemoryProperties = (PFN_vkGetPhysicalDeviceMemoryProperties)instanceLoadFn(m_instance, "vkGetPhysicalDeviceMemoryProperties");
        m_fn.vkGetPhysicalDeviceImageFormatProperties = (PFN_vkGetPhysicalDeviceImageFormatProperties)instanceLoadFn(m_instance, "vkGetPhysicalDeviceImageFormatProperties");
        m_fn.vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)instanceLoadFn(m_instance, "vkDestroySurfaceKHR");
        m_fn.vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)instanceLoadFn(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        m_fn.vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)instanceLoadFn(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
        m_fn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)instanceLoadFn(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");

        // device functions
        m_fn.vkAllocateCommandBuffers = (PFN_vkAllocateCommandBuffers)loadFn(m_device, "vkAllocateCommandBuffers");
        m_fn.vkAllocateDescriptorSets = (PFN_vkAllocateDescriptorSets)loadFn(m_device, "vkAllocateDescriptorSets");
        m_fn.vkAllocateMemory = (PFN_vkAllocateMemory)loadFn(m_device, "vkAllocateMemory");
        m_fn.vkBeginCommandBuffer = (PFN_vkBeginCommandBuffer)loadFn(m_device, "vkBeginCommandBuffer");
        m_fn.vkBindBufferMemory = (PFN_vkBindBufferMemory)loadFn(m_device, "vkBindBufferMemory");
        m_fn.vkBindImageMemory = (PFN_vkBindImageMemory)loadFn(m_device, "vkBindImageMemory");
        m_fn.vkCmdBeginQuery = (PFN_vkCmdBeginQuery)loadFn(m_device, "vkCmdBeginQuery");
        m_fn.vkCmdBeginRenderPass = (PFN_vkCmdBeginRenderPass)loadFn(m_device, "vkCmdBeginRenderPass");
        m_fn.vkCmdBindDescriptorSets = (PFN_vkCmdBindDescriptorSets)loadFn(m_device, "vkCmdBindDescriptorSets");
        m_fn.vkCmdBindIndexBuffer = (PFN_vkCmdBindIndexBuffer)loadFn(m_device, "vkCmdBindIndexBuffer");
        m_fn.vkCmdBindPipeline = (PFN_vkCmdBindPipeline)loadFn(m_device, "vkCmdBindPipeline");
        m_fn.vkCmdBindVertexBuffers = (PFN_vkCmdBindVertexBuffers)loadFn(m_device, "vkCmdBindVertexBuffers");
        m_fn.vkCmdBlitImage = (PFN_vkCmdBlitImage)loadFn(m_device, "vkCmdBlitImage");
        m_fn.vkCmdClearAttachments = (PFN_vkCmdClearAttachments)loadFn(m_device, "vkCmdClearAttachments");
        m_fn.vkCmdClearColorImage = (PFN_vkCmdClearColorImage)loadFn(m_device, "vkCmdClearColorImage");
        m_fn.vkCmdClearDepthStencilImage = (PFN_vkCmdClearDepthStencilImage)loadFn(m_device, "vkCmdClearDepthStencilImage");
        m_fn.vkCmdCopyBuffer = (PFN_vkCmdCopyBuffer)loadFn(m_device, "vkCmdCopyBuffer");
        m_fn.vkCmdCopyBufferToImage = (PFN_vkCmdCopyBufferToImage)loadFn(m_device, "vkCmdCopyBufferToImage");
        m_fn.vkCmdCopyImage = (PFN_vkCmdCopyImage)loadFn(m_device, "vkCmdCopyImage");
        m_fn.vkCmdCopyImageToBuffer = (PFN_vkCmdCopyImageToBuffer)loadFn(m_device, "vkCmdCopyImageToBuffer");
        m_fn.vkCmdCopyQueryPoolResults = (PFN_vkCmdCopyQueryPoolResults)loadFn(m_device, "vkCmdCopyQueryPoolResults");
        m_fn.vkCmdDispatch = (PFN_vkCmdDispatch)loadFn(m_device, "vkCmdDispatch");
        m_fn.vkCmdDispatchIndirect = (PFN_vkCmdDispatchIndirect)loadFn(m_device, "vkCmdDispatchIndirect");
        m_fn.vkCmdDraw = (PFN_vkCmdDraw)loadFn(m_device, "vkCmdDraw");
        m_fn.vkCmdDrawIndexed = (PFN_vkCmdDrawIndexed)loadFn(m_device, "vkCmdDrawIndexed");
        m_fn.vkCmdDrawIndexedIndirect = (PFN_vkCmdDrawIndexedIndirect)loadFn(m_device, "vkCmdDrawIndexedIndirect");
        m_fn.vkCmdDrawIndirect = (PFN_vkCmdDrawIndirect)loadFn(m_device, "vkCmdDrawIndirect");
        m_fn.vkCmdEndQuery = (PFN_vkCmdEndQuery)loadFn(m_device, "vkCmdEndQuery");
        m_fn.vkCmdEndRenderPass = (PFN_vkCmdEndRenderPass)loadFn(m_device, "vkCmdEndRenderPass");
        m_fn.vkCmdExecuteCommands = (PFN_vkCmdExecuteCommands)loadFn(m_device, "vkCmdExecuteCommands");
        m_fn.vkCmdFillBuffer = (PFN_vkCmdFillBuffer)loadFn(m_device, "vkCmdFillBuffer");
        m_fn.vkCmdNextSubpass = (PFN_vkCmdNextSubpass)loadFn(m_device, "vkCmdNextSubpass");
        m_fn.vkCmdPipelineBarrier = (PFN_vkCmdPipelineBarrier)loadFn(m_device, "vkCmdPipelineBarrier");
        m_fn.vkCmdPushConstants = (PFN_vkCmdPushConstants)loadFn(m_device, "vkCmdPushConstants");
        m_fn.vkCmdResetEvent = (PFN_vkCmdResetEvent)loadFn(m_device, "vkCmdResetEvent");
        m_fn.vkCmdResetQueryPool = (PFN_vkCmdResetQueryPool)loadFn(m_device, "vkCmdResetQueryPool");
        m_fn.vkCmdResolveImage = (PFN_vkCmdResolveImage)loadFn(m_device, "vkCmdResolveImage");
        m_fn.vkCmdSetBlendConstants = (PFN_vkCmdSetBlendConstants)loadFn(m_device, "vkCmdSetBlendConstants");
        m_fn.vkCmdSetDepthBias = (PFN_vkCmdSetDepthBias)loadFn(m_device, "vkCmdSetDepthBias");
        m_fn.vkCmdSetDepthBounds = (PFN_vkCmdSetDepthBounds)loadFn(m_device, "vkCmdSetDepthBounds");
        m_fn.vkCmdSetEvent = (PFN_vkCmdSetEvent)loadFn(m_device, "vkCmdSetEvent");
        m_fn.vkCmdSetLineWidth = (PFN_vkCmdSetLineWidth)loadFn(m_device, "vkCmdSetLineWidth");
        m_fn.vkCmdSetScissor = (PFN_vkCmdSetScissor)loadFn(m_device, "vkCmdSetScissor");
        m_fn.vkCmdSetStencilCompareMask = (PFN_vkCmdSetStencilCompareMask)loadFn(m_device, "vkCmdSetStencilCompareMask");
        m_fn.vkCmdSetStencilReference = (PFN_vkCmdSetStencilReference)loadFn(m_device, "vkCmdSetStencilReference");
        m_fn.vkCmdSetStencilWriteMask = (PFN_vkCmdSetStencilWriteMask)loadFn(m_device, "vkCmdSetStencilWriteMask");
        m_fn.vkCmdSetViewport = (PFN_vkCmdSetViewport)loadFn(m_device, "vkCmdSetViewport");
        m_fn.vkCmdUpdateBuffer = (PFN_vkCmdUpdateBuffer)loadFn(m_device, "vkCmdUpdateBuffer");
        m_fn.vkCmdWaitEvents = (PFN_vkCmdWaitEvents)loadFn(m_device, "vkCmdWaitEvents");
        m_fn.vkCmdWriteTimestamp = (PFN_vkCmdWriteTimestamp)loadFn(m_device, "vkCmdWriteTimestamp");
        m_fn.vkCreateBuffer = (PFN_vkCreateBuffer)loadFn(m_device, "vkCreateBuffer");
        m_fn.vkCreateBufferView = (PFN_vkCreateBufferView)loadFn(m_device, "vkCreateBufferView");
        m_fn.vkCreateCommandPool = (PFN_vkCreateCommandPool)loadFn(m_device, "vkCreateCommandPool");
        m_fn.vkCreateComputePipelines = (PFN_vkCreateComputePipelines)loadFn(m_device, "vkCreateComputePipelines");
        m_fn.vkCreateDescriptorPool = (PFN_vkCreateDescriptorPool)loadFn(m_device, "vkCreateDescriptorPool");
        m_fn.vkCreateDescriptorSetLayout = (PFN_vkCreateDescriptorSetLayout)loadFn(m_device, "vkCreateDescriptorSetLayout");
        m_fn.vkCreateEvent = (PFN_vkCreateEvent)loadFn(m_device, "vkCreateEvent");
        m_fn.vkCreateFence = (PFN_vkCreateFence)loadFn(m_device, "vkCreateFence");
        m_fn.vkCreateFramebuffer = (PFN_vkCreateFramebuffer)loadFn(m_device, "vkCreateFramebuffer");
        m_fn.vkCreateGraphicsPipelines = (PFN_vkCreateGraphicsPipelines)loadFn(m_device, "vkCreateGraphicsPipelines");
        m_fn.vkCreateImage = (PFN_vkCreateImage)loadFn(m_device, "vkCreateImage");
        m_fn.vkCreateImageView = (PFN_vkCreateImageView)loadFn(m_device, "vkCreateImageView");
        m_fn.vkCreatePipelineCache = (PFN_vkCreatePipelineCache)loadFn(m_device, "vkCreatePipelineCache");
        m_fn.vkCreatePipelineLayout = (PFN_vkCreatePipelineLayout)loadFn(m_device, "vkCreatePipelineLayout");
        m_fn.vkCreateQueryPool = (PFN_vkCreateQueryPool)loadFn(m_device, "vkCreateQueryPool");
        m_fn.vkCreateRenderPass = (PFN_vkCreateRenderPass)loadFn(m_device, "vkCreateRenderPass");
        m_fn.vkCreateSampler = (PFN_vkCreateSampler)loadFn(m_device, "vkCreateSampler");
        m_fn.vkCreateSemaphore = (PFN_vkCreateSemaphore)loadFn(m_device, "vkCreateSemaphore");
        m_fn.vkCreateShaderModule = (PFN_vkCreateShaderModule)loadFn(m_device, "vkCreateShaderModule");
        m_fn.vkDestroyBuffer = (PFN_vkDestroyBuffer)loadFn(m_device, "vkDestroyBuffer");
        m_fn.vkDestroyBufferView = (PFN_vkDestroyBufferView)loadFn(m_device, "vkDestroyBufferView");
        m_fn.vkDestroyCommandPool = (PFN_vkDestroyCommandPool)loadFn(m_device, "vkDestroyCommandPool");
        m_fn.vkDestroyDescriptorPool = (PFN_vkDestroyDescriptorPool)loadFn(m_device, "vkDestroyDescriptorPool");
        m_fn.vkDestroyDescriptorSetLayout = (PFN_vkDestroyDescriptorSetLayout)loadFn(m_device, "vkDestroyDescriptorSetLayout");
        m_fn.vkDestroyDevice = (PFN_vkDestroyDevice)loadFn(m_device, "vkDestroyDevice");
        m_fn.vkDestroyEvent = (PFN_vkDestroyEvent)loadFn(m_device, "vkDestroyEvent");
        m_fn.vkDestroyFence = (PFN_vkDestroyFence)loadFn(m_device, "vkDestroyFence");
        m_fn.vkDestroyFramebuffer = (PFN_vkDestroyFramebuffer)loadFn(m_device, "vkDestroyFramebuffer");
        m_fn.vkDestroyImage = (PFN_vkDestroyImage)loadFn(m_device, "vkDestroyImage");
        m_fn.vkDestroyImageView = (PFN_vkDestroyImageView)loadFn(m_device, "vkDestroyImageView");
        m_fn.vkDestroyPipeline = (PFN_vkDestroyPipeline)loadFn(m_device, "vkDestroyPipeline");
        m_fn.vkDestroyPipelineCache = (PFN_vkDestroyPipelineCache)loadFn(m_device, "vkDestroyPipelineCache");
        m_fn.vkDestroyPipelineLayout = (PFN_vkDestroyPipelineLayout)loadFn(m_device, "vkDestroyPipelineLayout");
        m_fn.vkDestroyQueryPool = (PFN_vkDestroyQueryPool)loadFn(m_device, "vkDestroyQueryPool");
        m_fn.vkDestroyRenderPass = (PFN_vkDestroyRenderPass)loadFn(m_device, "vkDestroyRenderPass");
        m_fn.vkDestroySampler = (PFN_vkDestroySampler)loadFn(m_device, "vkDestroySampler");
        m_fn.vkDestroySemaphore = (PFN_vkDestroySemaphore)loadFn(m_device, "vkDestroySemaphore");
        m_fn.vkDestroyShaderModule = (PFN_vkDestroyShaderModule)loadFn(m_device, "vkDestroyShaderModule");
        m_fn.vkDeviceWaitIdle = (PFN_vkDeviceWaitIdle)loadFn(m_device, "vkDeviceWaitIdle");
        m_fn.vkEndCommandBuffer = (PFN_vkEndCommandBuffer)loadFn(m_device, "vkEndCommandBuffer");
        m_fn.vkFlushMappedMemoryRanges = (PFN_vkFlushMappedMemoryRanges)loadFn(m_device, "vkFlushMappedMemoryRanges");
        m_fn.vkFreeCommandBuffers = (PFN_vkFreeCommandBuffers)loadFn(m_device, "vkFreeCommandBuffers");
        m_fn.vkFreeDescriptorSets = (PFN_vkFreeDescriptorSets)loadFn(m_device, "vkFreeDescriptorSets");
        m_fn.vkFreeMemory = (PFN_vkFreeMemory)loadFn(m_device, "vkFreeMemory");
        m_fn.vkGetBufferMemoryRequirements = (PFN_vkGetBufferMemoryRequirements)loadFn(m_device, "vkGetBufferMemoryRequirements");
        m_fn.vkGetDeviceMemoryCommitment = (PFN_vkGetDeviceMemoryCommitment)loadFn(m_device, "vkGetDeviceMemoryCommitment");
        m_fn.vkGetDeviceQueue = (PFN_vkGetDeviceQueue)loadFn(m_device, "vkGetDeviceQueue");
        m_fn.vkGetEventStatus = (PFN_vkGetEventStatus)loadFn(m_device, "vkGetEventStatus");
        m_fn.vkGetFenceStatus = (PFN_vkGetFenceStatus)loadFn(m_device, "vkGetFenceStatus");
        m_fn.vkGetImageMemoryRequirements = (PFN_vkGetImageMemoryRequirements)loadFn(m_device, "vkGetImageMemoryRequirements");
        m_fn.vkGetImageSparseMemoryRequirements = (PFN_vkGetImageSparseMemoryRequirements)loadFn(m_device, "vkGetImageSparseMemoryRequirements");
        m_fn.vkGetImageSubresourceLayout = (PFN_vkGetImageSubresourceLayout)loadFn(m_device, "vkGetImageSubresourceLayout");
        m_fn.vkGetPipelineCacheData = (PFN_vkGetPipelineCacheData)loadFn(m_device, "vkGetPipelineCacheData");
        m_fn.vkGetQueryPoolResults = (PFN_vkGetQueryPoolResults)loadFn(m_device, "vkGetQueryPoolResults");
        m_fn.vkGetRenderAreaGranularity = (PFN_vkGetRenderAreaGranularity)loadFn(m_device, "vkGetRenderAreaGranularity");
        m_fn.vkInvalidateMappedMemoryRanges = (PFN_vkInvalidateMappedMemoryRanges)loadFn(m_device, "vkInvalidateMappedMemoryRanges");
        m_fn.vkMapMemory = (PFN_vkMapMemory)loadFn(m_device, "vkMapMemory");
        m_fn.vkMergePipelineCaches = (PFN_vkMergePipelineCaches)loadFn(m_device, "vkMergePipelineCaches");
        m_fn.vkQueueBindSparse = (PFN_vkQueueBindSparse)loadFn(m_device, "vkQueueBindSparse");
        m_fn.vkQueueSubmit = (PFN_vkQueueSubmit)loadFn(m_device, "vkQueueSubmit");
        m_fn.vkQueueWaitIdle = (PFN_vkQueueWaitIdle)loadFn(m_device, "vkQueueWaitIdle");
        m_fn.vkResetCommandBuffer = (PFN_vkResetCommandBuffer)loadFn(m_device, "vkResetCommandBuffer");
        m_fn.vkResetCommandPool = (PFN_vkResetCommandPool)loadFn(m_device, "vkResetCommandPool");
        m_fn.vkResetDescriptorPool = (PFN_vkResetDescriptorPool)loadFn(m_device, "vkResetDescriptorPool");
        m_fn.vkResetEvent = (PFN_vkResetEvent)loadFn(m_device, "vkResetEvent");
        m_fn.vkResetFences = (PFN_vkResetFences)loadFn(m_device, "vkResetFences");
        m_fn.vkSetEvent = (PFN_vkSetEvent)loadFn(m_device, "vkSetEvent");
        m_fn.vkUnmapMemory = (PFN_vkUnmapMemory)loadFn(m_device, "vkUnmapMemory");
        m_fn.vkUpdateDescriptorSets = (PFN_vkUpdateDescriptorSets)loadFn(m_device, "vkUpdateDescriptorSets");
        m_fn.vkWaitForFences = (PFN_vkWaitForFences)loadFn(m_device, "vkWaitForFences");
        m_fn.vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)loadFn(m_device, "vkCreateSwapchainKHR");
        m_fn.vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)loadFn(m_device, "vkDestroySwapchainKHR");
        m_fn.vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)loadFn(m_device, "vkGetSwapchainImagesKHR");
        m_fn.vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)loadFn(m_device, "vkAcquireNextImageKHR");
        m_fn.vkQueuePresentKHR = (PFN_vkQueuePresentKHR)loadFn(m_device, "vkQueuePresentKHR");
    }

    void DeviceVK::initFunctions(const VulkanFunctions& fn)
    {
        // instance functions
        m_fn.vkGetPhysicalDeviceProperties =                fn.vkGetPhysicalDeviceProperties;
        m_fn.vkGetPhysicalDeviceMemoryProperties =          fn.vkGetPhysicalDeviceMemoryProperties;
        m_fn.vkGetPhysicalDeviceImageFormatProperties =     fn.vkGetPhysicalDeviceImageFormatProperties;
        m_fn.vkDestroySurfaceKHR =                          fn.vkDestroySurfaceKHR;
        m_fn.vkGetPhysicalDeviceSurfaceSupportKHR =         fn.vkGetPhysicalDeviceSurfaceSupportKHR;
        m_fn.vkGetPhysicalDeviceSurfaceFormatsKHR =         fn.vkGetPhysicalDeviceSurfaceFormatsKHR;
        m_fn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR =    fn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR;

        // device functions
        m_fn.vkAllocateCommandBuffers =                 fn.vkAllocateCommandBuffers;
        m_fn.vkAllocateDescriptorSets =                 fn.vkAllocateDescriptorSets;
        m_fn.vkAllocateMemory =                         fn.vkAllocateMemory;
        m_fn.vkBeginCommandBuffer =                     fn.vkBeginCommandBuffer;
        m_fn.vkBindBufferMemory =                       fn.vkBindBufferMemory;
        m_fn.vkBindImageMemory =                        fn.vkBindImageMemory;
        m_fn.vkCmdBeginQuery =                          fn.vkCmdBeginQuery;
        m_fn.vkCmdBeginRenderPass =                     fn.vkCmdBeginRenderPass;
        m_fn.vkCmdBindDescriptorSets =                  fn.vkCmdBindDescriptorSets;
        m_fn.vkCmdBindIndexBuffer =                     fn.vkCmdBindIndexBuffer;
        m_fn.vkCmdBindPipeline =                        fn.vkCmdBindPipeline;
        m_fn.vkCmdBindVertexBuffers =                   fn.vkCmdBindVertexBuffers;
        m_fn.vkCmdBlitImage =                           fn.vkCmdBlitImage;
        m_fn.vkCmdClearAttachments =                    fn.vkCmdClearAttachments;
        m_fn.vkCmdClearColorImage =                     fn.vkCmdClearColorImage;
        m_fn.vkCmdClearDepthStencilImage =              fn.vkCmdClearDepthStencilImage;
        m_fn.vkCmdCopyBuffer =                          fn.vkCmdCopyBuffer;
        m_fn.vkCmdCopyBufferToImage =                   fn.vkCmdCopyBufferToImage;
        m_fn.vkCmdCopyImage =                           fn.vkCmdCopyImage;
        m_fn.vkCmdCopyImageToBuffer =                   fn.vkCmdCopyImageToBuffer;
        m_fn.vkCmdCopyQueryPoolResults =                fn.vkCmdCopyQueryPoolResults;
        m_fn.vkCmdDispatch =                            fn.vkCmdDispatch;
        m_fn.vkCmdDispatchIndirect =                    fn.vkCmdDispatchIndirect;
        m_fn.vkCmdDraw =                                fn.vkCmdDraw;
        m_fn.vkCmdDrawIndexed =                         fn.vkCmdDrawIndexed;
        m_fn.vkCmdDrawIndexedIndirect =                 fn.vkCmdDrawIndexedIndirect;
        m_fn.vkCmdDrawIndirect =                        fn.vkCmdDrawIndirect;
        m_fn.vkCmdEndQuery =                            fn.vkCmdEndQuery;
        m_fn.vkCmdEndRenderPass =                       fn.vkCmdEndRenderPass;
        m_fn.vkCmdExecuteCommands =                     fn.vkCmdExecuteCommands;
        m_fn.vkCmdFillBuffer =                          fn.vkCmdFillBuffer;
        m_fn.vkCmdNextSubpass =                         fn.vkCmdNextSubpass;
        m_fn.vkCmdPipelineBarrier =                     fn.vkCmdPipelineBarrier;
        m_fn.vkCmdPushConstants =                       fn.vkCmdPushConstants;
        m_fn.vkCmdResetEvent =                          fn.vkCmdResetEvent;
        m_fn.vkCmdResetQueryPool =                      fn.vkCmdResetQueryPool;
        m_fn.vkCmdResolveImage =                        fn.vkCmdResolveImage;
        m_fn.vkCmdSetBlendConstants =                   fn.vkCmdSetBlendConstants;
        m_fn.vkCmdSetDepthBias =                        fn.vkCmdSetDepthBias;
        m_fn.vkCmdSetDepthBounds =                      fn.vkCmdSetDepthBounds;
        m_fn.vkCmdSetEvent =                            fn.vkCmdSetEvent;
        m_fn.vkCmdSetLineWidth =                        fn.vkCmdSetLineWidth;
        m_fn.vkCmdSetScissor =                          fn.vkCmdSetScissor;
        m_fn.vkCmdSetStencilCompareMask =               fn.vkCmdSetStencilCompareMask;
        m_fn.vkCmdSetStencilReference =                 fn.vkCmdSetStencilReference;
        m_fn.vkCmdSetStencilWriteMask =                 fn.vkCmdSetStencilWriteMask;
        m_fn.vkCmdSetViewport =                         fn.vkCmdSetViewport;
        m_fn.vkCmdUpdateBuffer =                        fn.vkCmdUpdateBuffer;
        m_fn.vkCmdWaitEvents =                          fn.vkCmdWaitEvents;
        m_fn.vkCmdWriteTimestamp =                      fn.vkCmdWriteTimestamp;
        m_fn.vkCreateBuffer =                           fn.vkCreateBuffer;
        m_fn.vkCreateBufferView =                       fn.vkCreateBufferView;
        m_fn.vkCreateCommandPool =                      fn.vkCreateCommandPool;
        m_fn.vkCreateComputePipelines =                 fn.vkCreateComputePipelines;
        m_fn.vkCreateDescriptorPool =                   fn.vkCreateDescriptorPool;
        m_fn.vkCreateDescriptorSetLayout =              fn.vkCreateDescriptorSetLayout;
        m_fn.vkCreateEvent =                            fn.vkCreateEvent;
        m_fn.vkCreateFence =                            fn.vkCreateFence;
        m_fn.vkCreateFramebuffer =                      fn.vkCreateFramebuffer;
        m_fn.vkCreateGraphicsPipelines =                fn.vkCreateGraphicsPipelines;
        m_fn.vkCreateImage =                            fn.vkCreateImage;
        m_fn.vkCreateImageView =                        fn.vkCreateImageView;
        m_fn.vkCreatePipelineCache =                    fn.vkCreatePipelineCache;
        m_fn.vkCreatePipelineLayout =                   fn.vkCreatePipelineLayout;
        m_fn.vkCreateQueryPool =                        fn.vkCreateQueryPool;
        m_fn.vkCreateRenderPass =                       fn.vkCreateRenderPass;
        m_fn.vkCreateSampler =                          fn.vkCreateSampler;
        m_fn.vkCreateSemaphore =                        fn.vkCreateSemaphore;
        m_fn.vkCreateShaderModule =                     fn.vkCreateShaderModule;
        m_fn.vkDestroyBuffer =                          fn.vkDestroyBuffer;
        m_fn.vkDestroyBufferView =                      fn.vkDestroyBufferView;
        m_fn.vkDestroyCommandPool =                     fn.vkDestroyCommandPool;
        m_fn.vkDestroyDescriptorPool =                  fn.vkDestroyDescriptorPool;
        m_fn.vkDestroyDescriptorSetLayout =             fn.vkDestroyDescriptorSetLayout;
        m_fn.vkDestroyDevice =                          fn.vkDestroyDevice;
        m_fn.vkDestroyEvent =                           fn.vkDestroyEvent;
        m_fn.vkDestroyFence =                           fn.vkDestroyFence;
        m_fn.vkDestroyFramebuffer =                     fn.vkDestroyFramebuffer;
        m_fn.vkDestroyImage =                           fn.vkDestroyImage;
        m_fn.vkDestroyImageView =                       fn.vkDestroyImageView;
        m_fn.vkDestroyPipeline =                        fn.vkDestroyPipeline;
        m_fn.vkDestroyPipelineCache =                   fn.vkDestroyPipelineCache;
        m_fn.vkDestroyPipelineLayout =                  fn.vkDestroyPipelineLayout;
        m_fn.vkDestroyQueryPool =                       fn.vkDestroyQueryPool;
        m_fn.vkDestroyRenderPass =                      fn.vkDestroyRenderPass;
        m_fn.vkDestroySampler =                         fn.vkDestroySampler;
        m_fn.vkDestroySemaphore =                       fn.vkDestroySemaphore;
        m_fn.vkDestroyShaderModule =                    fn.vkDestroyShaderModule;
        m_fn.vkDeviceWaitIdle =                         fn.vkDeviceWaitIdle;
        m_fn.vkEndCommandBuffer =                       fn.vkEndCommandBuffer;
        m_fn.vkFlushMappedMemoryRanges =                fn.vkFlushMappedMemoryRanges;
        m_fn.vkFreeCommandBuffers =                     fn.vkFreeCommandBuffers;
        m_fn.vkFreeDescriptorSets =                     fn.vkFreeDescriptorSets;
        m_fn.vkFreeMemory =                             fn.vkFreeMemory;
        m_fn.vkGetBufferMemoryRequirements =            fn.vkGetBufferMemoryRequirements;
        m_fn.vkGetDeviceMemoryCommitment =              fn.vkGetDeviceMemoryCommitment;
        m_fn.vkGetDeviceQueue =                         fn.vkGetDeviceQueue;
        m_fn.vkGetEventStatus =                         fn.vkGetEventStatus;
        m_fn.vkGetFenceStatus =                         fn.vkGetFenceStatus;
        m_fn.vkGetImageMemoryRequirements =             fn.vkGetImageMemoryRequirements;
        m_fn.vkGetImageSparseMemoryRequirements =       fn.vkGetImageSparseMemoryRequirements;
        m_fn.vkGetImageSubresourceLayout =              fn.vkGetImageSubresourceLayout;
        m_fn.vkGetPipelineCacheData =                   fn.vkGetPipelineCacheData;
        m_fn.vkGetQueryPoolResults =                    fn.vkGetQueryPoolResults;
        m_fn.vkGetRenderAreaGranularity =               fn.vkGetRenderAreaGranularity;
        m_fn.vkInvalidateMappedMemoryRanges =           fn.vkInvalidateMappedMemoryRanges;
        m_fn.vkMapMemory =                              fn.vkMapMemory;
        m_fn.vkMergePipelineCaches =                    fn.vkMergePipelineCaches;
        m_fn.vkQueueBindSparse =                        fn.vkQueueBindSparse;
        m_fn.vkQueueSubmit =                            fn.vkQueueSubmit;
        m_fn.vkQueueWaitIdle =                          fn.vkQueueWaitIdle;
        m_fn.vkResetCommandBuffer =                     fn.vkResetCommandBuffer;
        m_fn.vkResetCommandPool =                       fn.vkResetCommandPool;
        m_fn.vkResetDescriptorPool =                    fn.vkResetDescriptorPool;
        m_fn.vkResetEvent =                             fn.vkResetEvent;
        m_fn.vkResetFences =                            fn.vkResetFences;
        m_fn.vkSetEvent =                               fn.vkSetEvent;
        m_fn.vkUnmapMemory =                            fn.vkUnmapMemory;
        m_fn.vkUpdateDescriptorSets =                   fn.vkUpdateDescriptorSets;
        m_fn.vkWaitForFences =                          fn.vkWaitForFences;
        m_fn.vkCreateSwapchainKHR =                     fn.vkCreateSwapchainKHR;
        m_fn.vkDestroySwapchainKHR =                    fn.vkDestroySwapchainKHR;
        m_fn.vkGetSwapchainImagesKHR =                  fn.vkGetSwapchainImagesKHR;
        m_fn.vkAcquireNextImageKHR =                    fn.vkAcquireNextImageKHR;
        m_fn.vkQueuePresentKHR =                        fn.vkQueuePresentKHR;
    }
    
    void DeviceVK::initVma()
    {
        VmaVulkanFunctions fn{};
        fn.vkGetPhysicalDeviceProperties = m_fn.vkGetPhysicalDeviceProperties;
        fn.vkGetPhysicalDeviceMemoryProperties = m_fn.vkGetPhysicalDeviceMemoryProperties;
        fn.vkAllocateMemory = m_fn.vkAllocateMemory;
        fn.vkFreeMemory = m_fn.vkFreeMemory;
        fn.vkMapMemory = m_fn.vkMapMemory;
        fn.vkUnmapMemory = m_fn.vkUnmapMemory;
        fn.vkFlushMappedMemoryRanges = m_fn.vkFlushMappedMemoryRanges;
        fn.vkInvalidateMappedMemoryRanges = m_fn.vkInvalidateMappedMemoryRanges;
        fn.vkBindBufferMemory = m_fn.vkBindBufferMemory;
        fn.vkBindImageMemory = m_fn.vkBindImageMemory;
        fn.vkGetBufferMemoryRequirements = m_fn.vkGetBufferMemoryRequirements;
        fn.vkGetImageMemoryRequirements = m_fn.vkGetImageMemoryRequirements;
        fn.vkCreateBuffer = m_fn.vkCreateBuffer;
        fn.vkDestroyBuffer = m_fn.vkDestroyBuffer;
        fn.vkCreateImage = m_fn.vkCreateImage;
        fn.vkDestroyImage = m_fn.vkDestroyImage;
        fn.vkCmdCopyBuffer = m_fn.vkCmdCopyBuffer;

        VmaAllocatorCreateInfo vmaInfo{};
        vmaInfo.physicalDevice = m_physicalDevice;
        vmaInfo.device = m_device;
        vmaInfo.instance = m_instance;
        vmaInfo.pVulkanFunctions = &fn;

        if (KUAS_VULKAN_FAILED(vmaCreateAllocator(&vmaInfo, &m_allocator))) {
            KUAS_ASSERT(false && "Failed to create vulkan allocator");
        }
    }

    VkShaderModule DeviceVK::createShaderModule(size_t size, const uint32_t* code)
    {
        VkResult ret;
        VkShaderModule mod;
        VkShaderModuleCreateInfo smInfo{};

        smInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        smInfo.codeSize = size;
        smInfo.pCode = code;

        ret = m_fn.vkCreateShaderModule(m_device, &smInfo, nullptr, &mod);
        KUAS_ASSERT(!KUAS_VULKAN_FAILED(ret) && "Failed to create shader modules");

        return mod;
    }

#include "../shaders/Rect_VS.h"
#include "../shaders/Rect_GS.h"
#include "../shaders/Rect_FS.h"
#include "../shaders/RectAA_GS.h"
#include "../shaders/RectAA_FS.h"
#include "../shaders/Circle_VS.h"
#include "../shaders/Circle_GS.h"
#include "../shaders/Circle_FS.h"
#include "../shaders/CircleAA_GS.h"
#include "../shaders/CircleAA_FS.h"
#include "../shaders/RoundedRect_VS.h"
#include "../shaders/RoundedRect_GS.h"
#include "../shaders/RoundedRect_FS.h"
#include "../shaders/RoundedRectAA_GS.h"
#include "../shaders/RoundedRectAA_FS.h"
#include "../shaders/FillRect_VS.h"
#include "../shaders/FillRect_GS.h"
#include "../shaders/FillRect_FS.h"
#include "../shaders/FillRectAA_GS.h"
#include "../shaders/FillRectAA_FS.h"
#include "../shaders/FillRoundedRect_VS.h"
#include "../shaders/FillRoundedRect_GS.h"
#include "../shaders/FillRoundedRect_FS.h"
#include "../shaders/FillRoundedRectAA_GS.h"
#include "../shaders/FillRoundedRectAA_FS.h"
#include "../shaders/FillCircle_VS.h"
#include "../shaders/FillCircle_GS.h"
#include "../shaders/FillCircle_FS.h"
#include "../shaders/FillCircleAA_GS.h"
#include "../shaders/FillCircleAA_FS.h"
#include "../shaders/FillEllipse_VS.h"
#include "../shaders/FillEllipse_GS.h"
#include "../shaders/FillEllipseAA_GS.h"

    void DeviceVK::initShaderModule()
    {
        m_shaderModules[ShaderModuleID::Rect_VS] =
            createShaderModule(sizeof(ShaderRect_VS), ShaderRect_VS);

        m_shaderModules[ShaderModuleID::Rect_GS] =
            createShaderModule(sizeof(ShaderRect_GS), ShaderRect_GS);

        m_shaderModules[ShaderModuleID::Rect_PS] =
            createShaderModule(sizeof(ShaderRect_FS), ShaderRect_FS);

        m_shaderModules[ShaderModuleID::RectAA_GS] =
            createShaderModule(sizeof(ShaderRectAA_GS), ShaderRectAA_GS);

        m_shaderModules[ShaderModuleID::RectAA_PS] =
            createShaderModule(sizeof(ShaderRectAA_FS), ShaderRectAA_FS);

        m_shaderModules[ShaderModuleID::RoundedRect_VS] =
            createShaderModule(sizeof(ShaderRoundedRect_VS), ShaderRoundedRect_VS);

        m_shaderModules[ShaderModuleID::RoundedRect_GS] =
            createShaderModule(sizeof(ShaderRoundedRect_GS), ShaderRoundedRect_GS);

        m_shaderModules[ShaderModuleID::RoundedRect_PS] =
            createShaderModule(sizeof(ShaderRoundedRect_FS), ShaderRoundedRect_FS);

        m_shaderModules[ShaderModuleID::RoundedRectAA_GS] =
            createShaderModule(sizeof(ShaderRoundedRectAA_GS), ShaderRoundedRectAA_GS);

        m_shaderModules[ShaderModuleID::RoundedRectAA_PS] =
            createShaderModule(sizeof(ShaderRoundedRectAA_FS), ShaderRoundedRectAA_FS);

        m_shaderModules[ShaderModuleID::Circle_VS] =
            createShaderModule(sizeof(ShaderCircle_VS), ShaderCircle_VS);

        m_shaderModules[ShaderModuleID::Circle_GS] =
            createShaderModule(sizeof(ShaderCircle_GS), ShaderCircle_GS);

        m_shaderModules[ShaderModuleID::Circle_PS] =
            createShaderModule(sizeof(ShaderCircle_FS), ShaderCircle_FS);

        m_shaderModules[ShaderModuleID::CircleAA_GS] =
            createShaderModule(sizeof(ShaderCircleAA_GS), ShaderCircleAA_GS);

        m_shaderModules[ShaderModuleID::CircleAA_PS] =
            createShaderModule(sizeof(ShaderCircleAA_FS), ShaderCircleAA_FS);

        m_shaderModules[ShaderModuleID::FillRect_VS] =
            createShaderModule(sizeof(ShaderFillRect_VS), ShaderFillRect_VS);

        m_shaderModules[ShaderModuleID::FillRect_GS] =
            createShaderModule(sizeof(ShaderFillRect_GS), ShaderFillRect_GS);

        m_shaderModules[ShaderModuleID::FillRect_PS] =
            createShaderModule(sizeof(ShaderFillRect_FS), ShaderFillRect_FS);

        m_shaderModules[ShaderModuleID::FillRectAA_GS] =
            createShaderModule(sizeof(ShaderFillRectAA_GS), ShaderFillRectAA_GS);

        m_shaderModules[ShaderModuleID::FillRectAA_PS] =
            createShaderModule(sizeof(ShaderFillRectAA_FS), ShaderFillRectAA_FS);

        m_shaderModules[ShaderModuleID::FillRoundedRect_VS] =
            createShaderModule(sizeof(ShaderFillRoundedRect_VS), ShaderFillRoundedRect_VS);

        m_shaderModules[ShaderModuleID::FillRoundedRect_GS] =
            createShaderModule(sizeof(ShaderFillRoundedRect_GS), ShaderFillRoundedRect_GS);

        m_shaderModules[ShaderModuleID::FillRoundedRect_PS] =
            createShaderModule(sizeof(ShaderFillRoundedRect_FS), ShaderFillRoundedRect_FS);

        m_shaderModules[ShaderModuleID::FillRoundedRectAA_GS] =
            createShaderModule(sizeof(ShaderFillRoundedRectAA_GS), ShaderFillRoundedRectAA_GS);

        m_shaderModules[ShaderModuleID::FillRoundedRectAA_PS] =
            createShaderModule(sizeof(ShaderFillRoundedRectAA_FS), ShaderFillRoundedRectAA_FS);

        m_shaderModules[ShaderModuleID::FillCircle_VS] =
            createShaderModule(sizeof(ShaderFillCircle_VS), ShaderFillCircle_VS);

        m_shaderModules[ShaderModuleID::FillCircle_GS] =
            createShaderModule(sizeof(ShaderFillCircle_GS), ShaderFillCircle_GS);

        m_shaderModules[ShaderModuleID::FillCircle_PS] =
            createShaderModule(sizeof(ShaderFillCircle_FS), ShaderFillCircle_FS);

        m_shaderModules[ShaderModuleID::FillCircleAA_GS] =
            createShaderModule(sizeof(ShaderFillCircleAA_GS), ShaderFillCircleAA_GS);

        m_shaderModules[ShaderModuleID::FillCircleAA_PS] =
            createShaderModule(sizeof(ShaderFillCircleAA_FS), ShaderFillCircleAA_FS);

        m_shaderModules[ShaderModuleID::FillEllipse_VS] =
            createShaderModule(sizeof(ShaderFillEllipse_VS), ShaderFillEllipse_VS);

        m_shaderModules[ShaderModuleID::FillEllipse_GS] =
            createShaderModule(sizeof(ShaderFillEllipse_GS), ShaderFillEllipse_GS);

        m_shaderModules[ShaderModuleID::FillEllipseAA_GS] =
            createShaderModule(sizeof(ShaderFillEllipseAA_GS), ShaderFillEllipseAA_GS);
    }

#ifdef KUAS_USE_SDL
    bool DeviceVK::createSurfaceSDL(const SurfaceCreateDesc& desc, VkSurfaceKHR* surface)
    {
        return SDL_Vulkan_CreateSurface((SDL_Window*)desc.data0, m_instance, surface) == SDL_TRUE;
    }
#endif

#ifdef KUAS_PLATFORM_WINDOWS
    bool DeviceVK::createSurfaceWin32(const SurfaceCreateDesc& desc, VkSurfaceKHR* surface)
    {
        // TODO
        return false;
    }
#endif
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif // _MSC_VER
