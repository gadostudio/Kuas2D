#include "SemaphoreVK.h"
#include "DeviceVK.h"

namespace kuas
{
    SemaphoreVK::SemaphoreVK(VkSemaphore semaphore, DeviceVK* parentDevice) :
        m_semaphore(semaphore),
        m_parentDevice(parentDevice)
    {
    }

    SemaphoreVK::~SemaphoreVK()
    {
        m_parentDevice->getFunc().vkDestroySemaphore(
            m_parentDevice->getDevice(), m_semaphore, nullptr);
    }
}
