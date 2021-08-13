#include <kuas/BackendFactoryVK.h>
#include "vk/DeviceVK.h"
#include "vk/SemaphoreVK.h"

namespace kuas
{
    Result createBackendDeviceVk(const VulkanDeviceCreateDesc& desc, Device** device)
    {
        if (desc.instance == nullptr || desc.physicalDevice == nullptr
            || desc.device == nullptr)
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

    Result getBackendDeviceFnPtrsVk(Device* device, const VulkanFunctions& fnPtrs)
    {
        if (device == nullptr) {
            return Result::ErrInvalidArg;
        }

        std::memcpy(
            (void*)&fnPtrs,
            &KUAS_PTR_CAST(DeviceVK, device)->getFunc(),
            sizeof(VulkanFunctions));

        return Result::Ok;
    }

    Result createBackendImageVk(Device* device, const ImageCreateDesc& desc, VkImage vkImage, VkImageView view, Image** image)
    {
        if (device == nullptr || vkImage == VK_NULL_HANDLE
            || view == VK_NULL_HANDLE || image == nullptr)
        {
            return Result::ErrInvalidArg;
        }

        *image = new ImageVK(desc, vkImage, view, nullptr, KUAS_PTR_CAST(DeviceVK, device));
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
    
    Result createBackendSemaphoreVk(Device* device, VkSemaphore backendSemaphore, bool managed)
    {
        return Result::Ok;
    }

    VkSemaphore getBackendSemaphoreVk(Semaphore* semaphore)
    {
        if (semaphore == nullptr) {
            return VK_NULL_HANDLE;
        }

        return KUAS_PTR_CAST(SemaphoreVK, semaphore)->getHandle();
    }

}
