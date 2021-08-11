#pragma once

#include "VulkanHeaders.h"
#include <kuas/Types.h>

namespace kuas
{
    inline VkFormat convertPixelFormatVk(PixelFormat format)
    {
        static const VkFormat formatTable[] = {
            VK_FORMAT_UNDEFINED,                // Undefined,
            VK_FORMAT_R8_UNORM,                 // R8_Unorm,
            VK_FORMAT_R8_SRGB,                  // R8_Srgb,
            VK_FORMAT_R4G4_UNORM_PACK8,         // R4G4_Unorm,
            VK_FORMAT_R5G6B5_UNORM_PACK16,      // R5G6B5_Unorm,
            VK_FORMAT_R4G4B4A4_UNORM_PACK16,    // R4G4B4A4_Unorm,
            VK_FORMAT_R5G5B5A1_UNORM_PACK16,    // R5G5B5A1_Unorm,
            VK_FORMAT_A8B8G8R8_UNORM_PACK32,    // A8B8G8R8_Unorm,
            VK_FORMAT_A8B8G8R8_SRGB_PACK32,     // A8B8G8R8_Srgb,
            VK_FORMAT_B8G8R8A8_UNORM,           // B8G8R8A8_Unorm,
            VK_FORMAT_B8G8R8A8_SRGB,            // B8G8R8A8_Srgb,
            VK_FORMAT_R8G8B8A8_UNORM,           // R8G8B8A8_Unorm,
            VK_FORMAT_R8G8B8A8_SRGB,            // R8G8B8A8_Srgb,
            VK_FORMAT_A2R10G10B10_UNORM_PACK32, // A2R10G10B10_Unorm,
            VK_FORMAT_A2B10G10R10_UNORM_PACK32, // A2B10G10R10_Unorm,
            VK_FORMAT_R16G16_SFLOAT,            // R16G16_Float,
            VK_FORMAT_R16G16_UNORM,             // R16G16_Unorm,
            VK_FORMAT_R16G16B16A16_SFLOAT,      // R16G16B16A16_Float,
            VK_FORMAT_R16G16B16A16_UNORM,       // R16G16B16A16_Unorm,
            VK_FORMAT_R32G32_SFLOAT,            // R32G32_Float,
            VK_FORMAT_R32G32B32A32_SFLOAT       // R32G32B32A32_Float,
        };

        return formatTable[(uint32_t)format];
    }

    inline VkImageUsageFlags convertImageUsageVk(ImageUsageFlags usage)
    {
        VkImageUsageFlags ret = 0;

        if ((usage & ImageUsage::Drawing) == ImageUsage::Drawing) {
            ret |= VK_IMAGE_USAGE_SAMPLED_BIT;
        }

        if ((usage & ImageUsage::RenderTargetOutput) == ImageUsage::RenderTargetOutput) {
            ret |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        }

        if ((usage & ImageUsage::RenderTargetInput) == ImageUsage::RenderTargetInput) {
            ret |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }

        if ((usage & ImageUsage::TransferSrc) == ImageUsage::TransferSrc) {
            ret |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        }

        if ((usage & ImageUsage::TransferDst) == ImageUsage::TransferDst) {
            ret |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        }

        return ret;
    }

    inline VkAttachmentLoadOp convertLoadOpVk(AttachmentOp op)
    {
        switch (op) {
            case AttachmentOp::Load:
                return VK_ATTACHMENT_LOAD_OP_LOAD;
            case AttachmentOp::Clear:
                return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case AttachmentOp::DontCare:
                return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            default:
                break;
        }

        return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
    }

    inline VkAttachmentStoreOp convertStoreOpVk(AttachmentOp op)
    {
        switch (op) {
            case AttachmentOp::Store:
                return VK_ATTACHMENT_STORE_OP_STORE;
            case AttachmentOp::DontCare:
                return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            default:
                break;
        }

        return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
    }
}
