#include "FenceVK.h"
#include "DeviceVK.h"

namespace kuas
{
    FenceVK::FenceVK(VkFence fence, DeviceVK* parentDevice) :
        m_fence(fence),
        m_fn(parentDevice->getFunc()),
        m_parentDevice(parentDevice)
    {
    }

    FenceVK::~FenceVK()
    {
        m_parentDevice->getFunc().vkDestroyFence(
            m_parentDevice->getDevice(), m_fence, nullptr);
    }
    
    Result FenceVK::wait(uint64_t timeout)
    {
        VkResult result = m_fn.vkWaitForFences(
            m_parentDevice->getDevice(),
            1, &m_fence, VK_TRUE, timeout);

        switch (result) {
            case VK_SUCCESS:
                return Result::Ok;
            case VK_TIMEOUT:
                return Result::WaitTimeout;
            default:
                break;
        }

        return Result::ErrUnknown;
    }
    
    void FenceVK::reset()
    {
        m_fn.vkResetFences(m_parentDevice->getDevice(), 1, &m_fence);
    }

    FenceStatus FenceVK::getStatus()
    {
        VkResult result = m_fn.vkGetFenceStatus(m_parentDevice->getDevice(), m_fence);

        if (result == VK_SUCCESS) {
            return FenceStatus::Signaled;
        }

        return FenceStatus::Unsignaled;
    }
}
