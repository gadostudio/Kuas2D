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

        void resize(uint32_t w, uint32_t h) override;
        void swapBuffer() override;

        kuas::Ref<kuas::Device>& getKuasDevice() override { return m_ksDevice; }

    private:
        VulkanFn m_instfn;
        VkInstance m_instance;
        VkPhysicalDevice m_physicalDevice;
        VkDevice m_device;
        VkSurfaceKHR m_surface;
        uint32_t m_graphicsQueueFamily;
        VkQueue m_graphicsQueue;
        VkSwapchainKHR m_swapchain;
        VkCommandPool m_cmdPool;
        
        kuas::Ref<kuas::Device> m_ksDevice;

        struct BackbufferFormatMapping
        {
            VkFormat vkFormat;
            kuas::PixelFormat ksFormat;
        };

        BackbufferFormatMapping m_backbufferFmt;

        struct FrameData
        {
            VkImageView imageView;
            kuas::Ref<kuas::Image> renderTargetImage;
            kuas::Ref<kuas::RenderTarget> renderTarget;
            kuas::Ref<kuas::Semaphore> imageAcquiredSemaphore;
            kuas::Ref<kuas::Semaphore> renderFinishedSemaphore;
            kuas::Ref<kuas::Fence> frameFence;
            kuas::Ref<kuas::DrawList> drawList;
        };

        std::array<FrameData, 2> m_frameData;
        uint32_t m_currentFrame;

        kuas::Ref<kuas::DrawPass> m_drawPass;
        kuas::Ref<kuas::DrawList> m_drawList;

        void initInstance();
        void initDevice();
        void initSwapchain(SDL_Window* window);
        void initKuas();
        void initFrameData();
    };
}
