#pragma once

#include <kuas/DrawPass.h>
#include "VulkanHeaders.h"

namespace kuas
{
    class DeviceVK;

    class DrawPassVK : public DrawPass
    {
    public:
        DrawPassVK(VkRenderPass rp, DeviceVK* parentDevice);
        virtual ~DrawPassVK();

        VkRenderPass getHandle() { return m_renderPass; }

    private:
        VkRenderPass m_renderPass;
        DeviceVK* m_parentDevice;
    };
}
