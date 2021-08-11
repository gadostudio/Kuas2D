#pragma once

#include <kuas/SyncObjects.h>
#include <kuas/BackendFactoryVK.h>
#include "VulkanHeaders.h"

namespace kuas
{
    class DeviceVK;

    class FenceVK : public Fence
    {
    public:
        FenceVK(VkFence fence, DeviceVK* parentDevice);
        ~FenceVK();

        Result wait(uint64_t timeout = UINT64_MAX) override;
        void reset() override;
        FenceStatus getStatus() override;

        friend class DeviceVK;

    private:
        VkFence m_fence;
        const VulkanFunctions& m_fn;
        DeviceVK* m_parentDevice;
    };
}
