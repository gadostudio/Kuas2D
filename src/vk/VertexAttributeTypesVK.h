#pragma once

#include "../VertexAttributeTypes.h"
#include "VulkanHeaders.h"

namespace kuas
{
    struct BasicPolyFillVertexVK : public BasicPolyFillVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[2];
    };

    struct RectVertexVK : public RectVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[3];
    };

    struct RoundedRectVertexVK : public RoundedRectVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[4];
    };

    struct FillRectVertexVK : public FillRectVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[2];
    };

    struct FillRoundedRectVertexVK : public FillRoundedRectVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[3];
    };

    struct CircleVertexVK : public CircleVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[4];
    };

    struct FillCircleVertexVK : public FillCircleVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[3];
    };

    struct FillEllipseVertexVK : public FillEllipseVertex
    {
        static const VkVertexInputBindingDescription bindings[1];
        static const VkVertexInputAttributeDescription attributes[3];
    };
}
