#pragma once

#include <kuas/Surface.h>
#include <kuas/BackendFactoryVK.h>
#include <array>
#include "ImageVK.h"
#include "VulkanHeaders.h"

#define MAX_SWAPCHAIN_IMAGES 4

namespace kuas
{
    class DeviceVK;

    class SurfaceVK : public Surface
    {
    public:
        SurfaceVK(
            VkSurfaceKHR surface, 
            VkSwapchainKHR swapchain,
            ImageUsageFlags usage,
            PixelFormat pixFmt,
            VkColorSpaceKHR colorSpace,
            VkCommandPool cmdPool,
            DeviceVK* parentDevice);

        ~SurfaceVK();

        Result resize(uint32_t width, uint32_t height) override;
        Result swapBuffer(uint32_t numWaitSemaphores, Semaphore* const* semaphores) override;
        uint32_t getCurrentBackBuffer() const override;
        uint32_t getNumImages() const override;
        Image* getImage(uint32_t n) override;

    private:
        VkSurfaceKHR m_surface;
        VkSwapchainKHR m_swapchain;
        VkCommandPool m_cmdPool;
        ImageUsageFlags m_usage;
        PixelFormat m_pixFmt;
        VkColorSpaceKHR m_colorSpace;
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_surfaceWidth;
        uint32_t m_surfaceHeight;
        std::array<VkImage, MAX_SWAPCHAIN_IMAGES> m_surfaceImages{};
        std::array<ImageVK*, MAX_SWAPCHAIN_IMAGES> m_images{}; // facade images
        std::array<VkCommandBuffer, MAX_SWAPCHAIN_IMAGES> m_cmdBuffers{}; // pass-thru command buffers
        std::array<VkSemaphore, MAX_SWAPCHAIN_IMAGES> m_imageAcquiredSemaphores{};
        std::array<VkSemaphore, MAX_SWAPCHAIN_IMAGES> m_queueFinishedSemaphores{};
        std::array<VkFence, MAX_SWAPCHAIN_IMAGES> m_frameFences{};
        uint32_t m_numImages;
        uint32_t m_currentBackBuffer;
        const VulkanFunctions& m_fn;
        DeviceVK* m_parentDevice;

        void init(VkSwapchainKHR swapchain);
        void destroySwapchainObjects();
    };
}
