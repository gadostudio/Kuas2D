#pragma once

#include <kuas/Image.h>
#include "VulkanHeaders.h"

namespace kuas
{
    class DeviceVK;

    class ImageVK : public Image
    {
    public:
        ImageVK(
            const ImageCreateDesc& desc,
            VkImage image,
            VkImageView imageView,
            VmaAllocation allocation,
            DeviceVK* parentDevice);

        ~ImageVK();

        uint32_t getWidth() const override { return m_width; }
        uint32_t getHeight() const override { return m_height; }
        PixelFormat getFormat() const override { return m_format; }
        VkImage getImage() { return m_image; }
        VkImageView getImageView() { return m_imageView; }

        friend class DeviceVK;

    private:
        VkImage m_image;
        VkImageView m_imageView;
        VmaAllocation m_allocation;
        DeviceVK* m_parentDevice;
        
        uint32_t m_width;
        uint32_t m_height;
        PixelFormat m_format;
    };
}
