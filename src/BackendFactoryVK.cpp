#include <kuas/BackendFactoryVK.h>
#include "vk/DeviceVK.h"

namespace kuas
{
    Result createBackendDeviceVk(const VulkanDeviceCreateDesc& desc, Device** device)
    {
        if (desc.instance == nullptr || desc.physicalDevice == nullptr
            || desc.device == nullptr || desc.graphicsQueue == nullptr)
        {
            return Result::ErrInvalidArg;
        }

        if ((desc.deviceProcAddrFn == nullptr
            || desc.instanceProcAddrFn == nullptr)
            && desc.functions == nullptr)
        {
            return Result::ErrInvalidArg;
        }
        
        *device = new DeviceVK(desc);
        return Result::Ok;
    }

    Result createBackendImageVk(Device* device, ImageUsageFlags usage, Image** image)
    {
        return Result::Ok;
    }

    void getBackendImageVk(
        Image* image,
        VkImageUsageFlags* outUsageFlags,
        VkImageView* outImageView,
        VkImage* outBackendImage)
    {
        ImageVK* vkimage = KUAS_PTR_CAST(ImageVK, image);

        if (outImageView != nullptr) {
            *outImageView = vkimage->getImageView();
        }

        *outBackendImage = vkimage->getImage();
    }

    Result createBackendSemaphoreVk(Device** device, VkSemaphore backendSemaphore, bool managed)
    {

        return Result::Ok;
    }
}
