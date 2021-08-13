#pragma once

#include <kuas/SyncObjects.h>
#include "VulkanHeaders.h"

namespace kuas
{
    class DeviceVK;

    class SemaphoreVK : public Semaphore
    {
    public:
        SemaphoreVK(VkSemaphore semaphore, DeviceVK* parentDevice);
        ~SemaphoreVK();
        
        VkSemaphore getHandle() { return m_semaphore; }

        friend class DeviceVK;

    private:
        VkSemaphore m_semaphore;
        DeviceVK* m_parentDevice;
    };
}
