#include "ImageVK.h"
#include "DeviceVK.h"

namespace kuas
{
    ImageVK::ImageVK(
        const ImageCreateDesc& desc,
        VkImage image,
        VkImageView imageView,
        VmaAllocation allocation,
        DeviceVK* parentDevice) :
        m_image(image),
        m_imageView(imageView),
        m_allocation(allocation),
        m_parentDevice(parentDevice),
        m_width(desc.width),
        m_height(desc.height),
        m_format(desc.format)
    {
    }

    ImageVK::~ImageVK()
    {
        if (m_allocation != nullptr) {
            m_parentDevice->destroyImage(this);
        }
    }
}
