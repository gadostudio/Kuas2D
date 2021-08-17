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

    //

    const VkVertexInputBindingDescription RectVertexVK::bindings[1] = {
        { 0, sizeof(RectVertex), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription RectVertexVK::attributes[3] = {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RectVertex, posMin) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RectVertex, col) },
        { 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(RectVertex, thickness) }
    };

    //

    const VkVertexInputBindingDescription RoundedRectVertexVK::bindings[1] = {
        { 0, sizeof(RoundedRectVertex), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription RoundedRectVertexVK::attributes[4] = {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RoundedRectVertex, posMin) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(RoundedRectVertex, col) },
        { 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(RoundedRectVertex, thickness) },
        { 3, 0, VK_FORMAT_R32_SFLOAT, offsetof(RoundedRectVertex, roundness) },
    };

    //

    const VkVertexInputBindingDescription FillRectVertexVK::bindings[1] = {
        { 0, sizeof(FillRectVertex), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription FillRectVertexVK::attributes[2] = {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(FillRectVertex, posMin) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(FillRectVertex, col) }
    };

    //

    const VkVertexInputBindingDescription FillRoundedRectVertexVK::bindings[1] = {
        { 0, sizeof(FillRoundedRectVertex), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription FillRoundedRectVertexVK::attributes[3] = {
        { 0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(FillRoundedRectVertex, posMin) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(FillRoundedRectVertex, col) },
        { 3, 0, VK_FORMAT_R32_SFLOAT, offsetof(FillRoundedRectVertex, roundness) }
    };

    //

    const VkVertexInputBindingDescription CircleVertexVK::bindings[1] = {
        { 0, sizeof(CircleVertex), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription CircleVertexVK::attributes[4] = {
        { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(CircleVertexVK, pos) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(CircleVertexVK, col) },
        { 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(CircleVertexVK, radius) },
        { 3, 0, VK_FORMAT_R32_SFLOAT, offsetof(CircleVertexVK, thickness) }
    };

    //

    const VkVertexInputBindingDescription FillCircleVertexVK::bindings[1] = {
        { 0, sizeof(FillCircleVertex), VK_VERTEX_INPUT_RATE_VERTEX }
    };

    const VkVertexInputAttributeDescription FillCircleVertexVK::attributes[3] = {
        { 0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(FillCircleVertexVK, pos) },
        { 1, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(FillCircleVertexVK, col) },
        { 2, 0, VK_FORMAT_R32_SFLOAT, offsetof(FillCircleVertexVK, radius) }
    };
}
