#pragma once

#include <kuas/RenderTarget.h>
#include "ImageVK.h"
#include "VulkanHeaders.h"

namespace kuas
{
    class DeviceVK;

    class RenderTargetVK : public RenderTarget
    {
    public:
        RenderTargetVK(
            VkFramebuffer framebuffer,
            uint32_t width, uint32_t height,
            DeviceVK* parentDevice);
        virtual ~RenderTargetVK();

        uint32_t getWidth() const override { return m_width; }
        uint32_t getHeight() const override { return m_height; }
        VkFramebuffer getHandle() { return m_framebuffer; }

    private:
        VkFramebuffer m_framebuffer;
        DeviceVK* m_parentDevice;
        uint32_t m_width;
        uint32_t m_height;
    };
}
