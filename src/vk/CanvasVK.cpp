#include "CanvasVK.h"
#include "DeviceVK.h"

namespace kuas
{
    CanvasVK::CanvasVK(
        VkFramebuffer framebuffer,
        uint32_t width, uint32_t height,
        DeviceVK* parentDevice) :
        m_framebuffer(framebuffer),
        m_width(width),
        m_height(height),
        m_parentDevice(parentDevice)
    {
    }

    CanvasVK::~CanvasVK()
    {
        m_parentDevice->getFunc().vkDestroyFramebuffer(
            m_parentDevice->getDevice(), m_framebuffer, nullptr);
    }
}
