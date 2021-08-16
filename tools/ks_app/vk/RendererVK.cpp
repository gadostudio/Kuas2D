#include "RendererVK.h"
#include <SDL_vulkan.h>

namespace ks_app
{
    RendererVK::RendererVK(SDL_Window* window, uint32_t w, uint32_t h) :
        m_instfn{},
        m_instance(VK_NULL_HANDLE),
        m_device(VK_NULL_HANDLE),
        m_graphicsQueueFamily(0),
        m_graphicsQueue(VK_NULL_HANDLE)
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
    }

    RendererVK::~RendererVK()
    {
        m_ksDevice->waitIdle();
        m_ksDevice.reset();

        m_instfn.vkDestroyDevice(m_device, nullptr);
        m_instfn.vkDestroyInstance(m_instance, nullptr);
        SDL_Vulkan_UnloadLibrary();
    }

    void RendererVK::initInstance()
    {
        static const char* instanceLayers[] = {
            "VK_LAYER_KHRONOS_validation",
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

        m_instfn.vkGetDeviceProcAddr = (PFN_vkGetDeviceProcAddr)m_instfn.vkGetInstanceProcAddr(m_instance, "vkGetDeviceProcAddr");
        m_instfn.vkDestroyDevice = (PFN_vkDestroyDevice)m_instfn.vkGetDeviceProcAddr(m_device, "vkDestroyDevice");
        m_instfn.vkGetDeviceQueue = (PFN_vkGetDeviceQueue)m_instfn.vkGetDeviceProcAddr(m_device, "vkGetDeviceQueue");

        m_instfn.vkGetDeviceQueue(m_device, m_graphicsQueueFamily, 0, &m_graphicsQueue);
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
}