#pragma once

#include "VertexAttributeTypesVK.h"

namespace kuas
{
    const VkVertexInputBindingDescription BasicPolyFillVertexVK::bindings[1] = {
        { 0, sizeof(BasicPolyFillVertexVK), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription BasicPolyFillVertexVK::attributes[2] = {
        { 0, 0, VK_FORMAT_R32G32_SFLOAT, 0 },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(Vec2F) }
    };

    const VkVertexInputBindingDescription RectVertexVK::bindings[1] = {
        { 0, sizeof(RectVertexVK), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription RectVertexVK::attributes[3] = {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RectVertex, posMin) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RectVertex, col) },
        { 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(RectVertex, thickness) }
    };

    const VkVertexInputBindingDescription FillRectVertexVK::bindings[1] = {
        { 0, sizeof(FillRectVertexVK), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription FillRectVertexVK::attributes[2] = {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RectVertex, posMin) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RectVertex, col) }
    };
}
