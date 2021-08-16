#pragma once

#define VK_NO_PROTOTYPES

#include "../Renderer.h"
#include <array>
#include <SDL.h>
#include <kuas/BackendFactoryVK.h>

#undef main

#define KS_APP_VK_FAILED(x) \
    ((x) < VK_SUCCESS)

namespace ks_app
{
    struct VulkanFn
    {
        PFN_vkCreateInstance vkCreateInstance;
        PFN_vkDestroyInstance vkDestroyInstance;
        PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices;
        PFN_vkGetPhysicalDeviceFeatures vkGetPhysicalDeviceFeatures;
        PFN_vkGetPhysicalDeviceQueueFamilyProperties vkGetPhysicalDeviceQueueFamilyProperties;
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
        PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;
        PFN_vkCreateDevice vkCreateDevice;
        PFN_vkDestroyDevice vkDestroyDevice;
        PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR;
        PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
        PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
        PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
        PFN_vkGetDeviceQueue vkGetDeviceQueue;
        PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR;
        PFN_vkGetPhysicalDeviceSurfaceFormatsKHR vkGetPhysicalDeviceSurfaceFormatsKHR;
        PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR vkGetPhysicalDeviceSurfaceCapabilitiesKHR;
        PFN_vkCreateImageView vkCreateImageView;
        PFN_vkDestroyImageView vkDestroyImageView;
        PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR;
        PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR;
        PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR;
        PFN_vkAcquireNextImageKHR vkAcquireNextImageKHR;
        PFN_vkQueuePresentKHR vkQueuePresentKHR;
    };

    class RendererVK : public Renderer
    {
    public:
        RendererVK(SDL_Window* window, uint32_t w, uint32_t h);
        ~RendererVK();

        kuas::Ref<kuas::Device>& getKuasDevice() override { return m_ksDevice; }

    private:
        VulkanFn m_instfn;
        VkInstance m_instance;
        VkPhysicalDevice m_physicalDevice;
        VkDevice m_device;
        uint32_t m_graphicsQueueFamily;
        VkQueue m_graphicsQueue;
        kuas::Ref<kuas::Device> m_ksDevice;

        void initInstance();
        void initDevice();
        void initKuas();
    };
}
