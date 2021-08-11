#include "DrawPassVK.h"
#include "DeviceVK.h"

namespace kuas
{
    DrawPassVK::DrawPassVK(VkRenderPass rp, DeviceVK* parentDevice) :
        m_renderPass(rp),
        m_parentDevice(parentDevice)
    {
    }

    DrawPassVK::~DrawPassVK()
    {
        m_parentDevice->getFunc().vkDestroyRenderPass(
            m_parentDevice->getDevice(), m_renderPass, nullptr);
    }
}
