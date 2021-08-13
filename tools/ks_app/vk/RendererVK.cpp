#include "RendererVK.h"
#include <SDL_vulkan.h>

namespace ks_app
{
    RendererVK::RendererVK(SDL_Window* window, uint32_t w, uint32_t h) :
        m_instfn{},
        m_instance(VK_NULL_HANDLE),
        m_device(VK_NULL_HANDLE),
        m_surface(VK_NULL_HANDLE),
        m_graphicsQueueFamily(0),
        m_graphicsQueue(VK_NULL_HANDLE),
        m_swapchain(VK_NULL_HANDLE),
        m_currentFrame(0)
    {
        if (SDL_Vulkan_LoadLibrary(nullptr) < 0) {
            throw std::runtime_error("Cannot load vulkan library");
        }

        PFN_vkGetInstanceProcAddr fnGetInstanceProcAddr =
            (PFN_vkGetInstanceProcAddr)SDL_Vulkan_GetVkGetInstanceProcAddr();
    
        m_instfn.vkGetInstanceProcAddr = fnGetInstanceProcAddr;
        m_instfn.vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)fnGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion");
        m_instfn.vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)fnGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties");
        m_instfn.vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)fnGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties");
        m_instfn.vkCreateInstance = (PFN_vkCreateInstance)fnGetInstanceProcAddr(nullptr, "vkCreateInstance");

        initInstance();
        initDevice();
        initKuas();
        initSwapchain(window);

        
    }

    RendererVK::~RendererVK()
    {
        for (auto& frameData : m_frameData) {
            frameData.imageAcquiredSemaphore.reset();
            frameData.renderFinishedSemaphore.reset();
            frameData.renderTargetImage.reset();
            frameData.renderTarget.reset();
            m_instfn.vkDestroyImageView(m_device, frameData.imageView, nullptr);
        }

        m_ksDevice.reset();

        m_instfn.vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        m_instfn.vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        m_instfn.vkDestroyDevice(m_device, nullptr);
        m_instfn.vkDestroyInstance(m_instance, nullptr);
        SDL_Vulkan_UnloadLibrary();
    }

    void RendererVK::resize(uint32_t w, uint32_t h)
    {
    }

    void RendererVK::swapBuffer()
    {
        uint32_t prevFrame = (m_currentFrame - 1) % 2;
        FrameData& frameData = m_frameData[m_currentFrame];
        FrameData& prevFrameData = m_frameData[prevFrame];
        VkSemaphore imageAcquired = kuas::getBackendSemaphoreVk(frameData.imageAcquiredSemaphore.get());
        VkSemaphore renderFinished = kuas::getBackendSemaphoreVk(frameData.renderFinishedSemaphore.get());

        m_instfn.vkAcquireNextImageKHR(
            m_device, m_swapchain, UINT64_MAX,
            imageAcquired,
            nullptr, &m_currentFrame);

        prevFrameData.frameFence->wait();
        prevFrameData.frameFence->reset();

        m_ksDevice->enqueueWait(1, &frameData.imageAcquiredSemaphore);
        m_ksDevice->enqueueDrawLists(1, &frameData.drawList);
        m_ksDevice->enqueueSignal(1, &frameData.renderFinishedSemaphore, frameData.frameFence.get());

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderFinished;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapchain;
        presentInfo.pImageIndices = &m_currentFrame;

        m_instfn.vkQueuePresentKHR(m_graphicsQueue, &presentInfo);

        m_currentFrame = (m_currentFrame + 1) % 2;
    }

    void RendererVK::initInstance()
    {
        static const char* instanceLayers[] = {
#ifndef NDEBUG
            "VK_LAYER_KHRONOS_validation",
#endif
        };

        static const char* instanceExts[] = {
            "VK_KHR_surface",
            "VK_KHR_win32_surface"
        };

        uint32_t numLayers;
        std::vector<VkLayerProperties> layers;
        std::vector<const char*> enabledLayers;

        m_instfn.vkEnumerateInstanceLayerProperties(&numLayers, nullptr);
        layers.resize(numLayers);
        m_instfn.vkEnumerateInstanceLayerProperties(&numLayers, layers.data());

        for (auto layer : instanceLayers) {
            for (const auto& l : layers) {
                if (strncmp(layer, l.layerName, VK_MAX_DESCRIPTION_SIZE) == 0) {
                    enabledLayers.push_back(layer);
                }
            }
        }

        uint32_t numExtensions;
        std::vector<VkExtensionProperties> extensions;
        std::vector<const char*> enabledExtensions;

        m_instfn.vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, nullptr);
        extensions.resize(numExtensions);
        m_instfn.vkEnumerateInstanceExtensionProperties(nullptr, &numExtensions, extensions.data());

        for (auto extension : instanceExts) {
            for (const auto& e : extensions) {
                if (strncmp(extension, e.extensionName, VK_MAX_DESCRIPTION_SIZE) == 0) {
                    enabledExtensions.push_back(extension);
                }
            }
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "ks_app";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "ks_app";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        if (m_instfn.vkEnumerateInstanceVersion != nullptr) {
            m_instfn.vkEnumerateInstanceVersion(&appInfo.apiVersion);
        }

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
        instanceInfo.ppEnabledLayerNames = enabledLayers.data();
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
        instanceInfo.ppEnabledExtensionNames = enabledExtensions.data();

        if (KS_APP_VK_FAILED(m_instfn.vkCreateInstance(&instanceInfo, nullptr, &m_instance))) {
            throw std::runtime_error("Cannot create vulkan instance");
        }

        m_instfn.vkDestroyInstance = (PFN_vkDestroyInstance)m_instfn.vkGetInstanceProcAddr(m_instance, "vkDestroyInstance");
        m_instfn.vkEnumeratePhysicalDevices = (PFN_vkEnumeratePhysicalDevices)m_instfn.vkGetInstanceProcAddr(m_instance, "vkEnumeratePhysicalDevices");
        m_instfn.vkGetPhysicalDeviceFeatures = (PFN_vkGetPhysicalDeviceFeatures)m_instfn.vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceFeatures");
        m_instfn.vkGetPhysicalDeviceQueueFamilyProperties = (PFN_vkGetPhysicalDeviceQueueFamilyProperties)m_instfn.vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceQueueFamilyProperties");
        m_instfn.vkCreateDevice = (PFN_vkCreateDevice)m_instfn.vkGetInstanceProcAddr(m_instance, "vkCreateDevice");
        m_instfn.vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)m_instfn.vkGetInstanceProcAddr(m_instance, "vkGetDeviceProcAddr");
        m_instfn.vkDestroySurfaceKHR = (PFN_vkDestroySurfaceKHR)m_instfn.vkGetInstanceProcAddr(m_instance, "vkDestroySurfaceKHR");
        m_instfn.vkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)m_instfn.vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
        m_instfn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)m_instfn.vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
        m_instfn.vkGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)m_instfn.vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
    }
    
    void RendererVK::initDevice()
    {
        uint32_t numPhysicalDevices;
        std::vector<VkPhysicalDevice> physicalDevices;
        VkPhysicalDeviceFeatures features;

        m_instfn.vkEnumeratePhysicalDevices(m_instance, &numPhysicalDevices, nullptr);
        physicalDevices.resize(numPhysicalDevices);
        m_instfn.vkEnumeratePhysicalDevices(m_instance, &numPhysicalDevices, physicalDevices.data());

        m_physicalDevice = physicalDevices[0];
        m_instfn.vkGetPhysicalDeviceFeatures(m_physicalDevice, &features);

        uint32_t numFamilies;
        uint32_t queueFamilyIndex = 0;
        std::vector<VkQueueFamilyProperties> queueFamilies;

        m_instfn.vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &numFamilies, nullptr);
        queueFamilies.resize(numFamilies);
        m_instfn.vkGetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &numFamilies, queueFamilies.data());

        for (const auto& q : queueFamilies) {
            if ((q.queueFlags & VK_QUEUE_GRAPHICS_BIT) == VK_QUEUE_GRAPHICS_BIT) {
                break;
            }
            queueFamilyIndex++;
        }

        m_graphicsQueueFamily = queueFamilyIndex;

        static const float queuePriority = 1.0f;

        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = queueFamilyIndex;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;

        static const char* swapchainExtension[] = {
            "VK_KHR_swapchain"
        };

        VkDeviceCreateInfo deviceInfo{};
        deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        deviceInfo.queueCreateInfoCount = 1;
        deviceInfo.pQueueCreateInfos = &queueInfo;
        deviceInfo.enabledExtensionCount = KUAS_ARRAY_SIZE(swapchainExtension);
        deviceInfo.ppEnabledExtensionNames = swapchainExtension;
        deviceInfo.pEnabledFeatures = &features;

        if (KS_APP_VK_FAILED(m_instfn.vkCreateDevice(m_physicalDevice, &deviceInfo, nullptr, &m_device))) {
            throw std::runtime_error("Cannot create device");
        }

        m_instfn.vkDestroyDevice = (PFN_vkDestroyDevice)m_instfn.vkGetDeviceProcAddr(m_device, "vkDestroyDevice");
        m_instfn.vkGetDeviceQueue = (PFN_vkGetDeviceQueue)m_instfn.vkGetDeviceProcAddr(m_device, "vkGetDeviceQueue");
        m_instfn.vkCreateImageView = (PFN_vkCreateImageView)m_instfn.vkGetDeviceProcAddr(m_device, "vkCreateImageView");
        m_instfn.vkDestroyImageView = (PFN_vkDestroyImageView)m_instfn.vkGetDeviceProcAddr(m_device, "vkDestroyImageView");
        m_instfn.vkCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)m_instfn.vkGetDeviceProcAddr(m_device, "vkCreateSwapchainKHR");
        m_instfn.vkDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)m_instfn.vkGetDeviceProcAddr(m_device, "vkDestroySwapchainKHR");
        m_instfn.vkGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)m_instfn.vkGetDeviceProcAddr(m_device, "vkGetSwapchainImagesKHR");
        m_instfn.vkAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)m_instfn.vkGetDeviceProcAddr(m_device, "vkAcquireNextImageKHR");
        m_instfn.vkQueuePresentKHR = (PFN_vkQueuePresentKHR)m_instfn.vkGetDeviceProcAddr(m_device, "vkQueuePresentKHR");

        m_instfn.vkGetDeviceQueue(m_device, m_graphicsQueueFamily, 0, &m_graphicsQueue);
    }

    void RendererVK::initSwapchain(SDL_Window* window)
    {
        static const BackbufferFormatMapping formats[] = {
            { VK_FORMAT_R8G8B8A8_SRGB, kuas::PixelFormat::R8G8B8A8_Srgb },
            { VK_FORMAT_R8G8B8A8_UNORM, kuas::PixelFormat::R8G8B8A8_Unorm },
            { VK_FORMAT_B8G8R8A8_SRGB, kuas::PixelFormat::B8G8R8A8_Srgb },
            { VK_FORMAT_B8G8R8A8_UNORM, kuas::PixelFormat::B8G8R8A8_Unorm }
        };

        SDL_Vulkan_CreateSurface(window, m_instance, &m_surface);

        // we assume this graphics queue family has support for surface presentation.
        VkBool32 surfaceSupport;
        m_instfn.vkGetPhysicalDeviceSurfaceSupportKHR(m_physicalDevice, m_graphicsQueueFamily, m_surface, &surfaceSupport);
        KUAS_ASSERT(surfaceSupport == VK_TRUE);

        VkSurfaceCapabilitiesKHR surfCaps;
        m_instfn.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfCaps);

        uint32_t numFormats;
        std::vector<VkSurfaceFormatKHR> supportedFormats;

        m_instfn.vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &numFormats, nullptr);
        supportedFormats.resize(numFormats);
        m_instfn.vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &numFormats, supportedFormats.data());

        VkFormat choosenFormat = VK_FORMAT_UNDEFINED;
        VkColorSpaceKHR colorSpace = VK_COLOR_SPACE_MAX_ENUM_KHR;

        for (const auto& fmt : formats) {
            for (const auto& supportedFmt : supportedFormats) {
                if (fmt.vkFormat == supportedFmt.format) {
                    choosenFormat = fmt.vkFormat;
                    colorSpace = supportedFmt.colorSpace;
                    m_backbufferFmt = fmt;
                }
            }
        }

        VkSwapchainCreateInfoKHR swapchainInfo{};
        swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        swapchainInfo.surface = m_surface;
        swapchainInfo.minImageCount = 2;
        swapchainInfo.imageFormat = choosenFormat;
        swapchainInfo.imageColorSpace = colorSpace;
        swapchainInfo.imageExtent.width = surfCaps.maxImageExtent.width;
        swapchainInfo.imageExtent.height = surfCaps.maxImageExtent.height;
        swapchainInfo.imageArrayLayers = 1;
        swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
        swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        swapchainInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
        swapchainInfo.clipped = VK_TRUE;
        swapchainInfo.oldSwapchain = VK_NULL_HANDLE;

        if (KS_APP_VK_FAILED(m_instfn.vkCreateSwapchainKHR(m_device, &swapchainInfo, nullptr, &m_swapchain))) {
            throw std::runtime_error("Cannot create swapchain");
        }

        uint32_t numImages = 2;
        std::array<VkImage, 2> images;

        m_instfn.vkGetSwapchainImagesKHR(m_device, m_swapchain, &numImages, images.data());

        kuas::DrawPassCreateDesc drawPassDesc{};
        drawPassDesc.format = m_backbufferFmt.ksFormat;
        drawPassDesc.loadOp = kuas::AttachmentOp::Clear;
        drawPassDesc.storeOp = kuas::AttachmentOp::Store;

        m_ksDevice->createDrawPass(drawPassDesc, &m_drawPass);

        for (uint32_t i = 0; i < numImages; i++)
        {
            FrameData& frameData = m_frameData[i];

            VkImageViewCreateInfo imgView{};
            imgView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imgView.image = images[i];
            imgView.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imgView.format = m_backbufferFmt.vkFormat;
            imgView.components.r = VK_COMPONENT_SWIZZLE_R;
            imgView.components.g = VK_COMPONENT_SWIZZLE_G;
            imgView.components.b = VK_COMPONENT_SWIZZLE_B;
            imgView.components.a = VK_COMPONENT_SWIZZLE_A;
            imgView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            imgView.subresourceRange.baseMipLevel = 0;
            imgView.subresourceRange.levelCount = 1;
            imgView.subresourceRange.baseArrayLayer = 0;
            imgView.subresourceRange.layerCount = 1;

            if (KS_APP_VK_FAILED(m_instfn.vkCreateImageView(m_device, &imgView, nullptr, &frameData.imageView))) {
                throw std::runtime_error("Cannot create image view");
            }

            kuas::ImageCreateDesc imageDesc{};
            imageDesc.format = m_backbufferFmt.ksFormat;
            imageDesc.width = surfCaps.maxImageExtent.width;
            imageDesc.height = surfCaps.maxImageExtent.height;
            imageDesc.usage = kuas::ImageUsage::RenderTargetOutput;
            
            if (KUAS_FAILED(kuas::createBackendImageVk(
                m_ksDevice.get(), imageDesc, images[i],
                frameData.imageView, &frameData.renderTargetImage)))
            {
                throw std::runtime_error("Cannot create image");
            }

            m_ksDevice->createRenderTarget(
                m_drawPass.get(), frameData.renderTargetImage.get(),
                imageDesc.width, imageDesc.height, &frameData.renderTarget);

            m_ksDevice->createSemaphore(&frameData.imageAcquiredSemaphore);
            m_ksDevice->createSemaphore(&frameData.renderFinishedSemaphore);
            m_ksDevice->createFence(false, &frameData.frameFence);

            kuas::ColorRGBA color(1.0f, 0.0f, 0.0f);

            m_ksDevice->createDrawList(&frameData.drawList);
            frameData.drawList->begin();
            frameData.drawList->beginDrawPass(m_drawPass.get(), frameData.renderTarget.get(), &color);
            frameData.drawList->endDrawPass();
            frameData.drawList->end();
        }
    }

    void RendererVK::initKuas()
    {
        kuas::VulkanDeviceCreateDesc deviceDesc{};
        deviceDesc.instance = m_instance;
        deviceDesc.physicalDevice = m_physicalDevice;
        deviceDesc.device = m_device;
        deviceDesc.graphicsQueueFamilyIndex = m_graphicsQueueFamily;
        deviceDesc.instanceProcAddrFn = m_instfn.vkGetInstanceProcAddr;
        deviceDesc.deviceProcAddrFn = m_instfn.vkGetDeviceProcAddr;

        if (KUAS_FAILED(kuas::createBackendDeviceVk(deviceDesc, &m_ksDevice))) {
            throw std::runtime_error("Cannot create vulkan backend");
        }
    }

    void RendererVK::initFrameData()
    {
        
    }
}