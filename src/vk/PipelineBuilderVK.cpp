#include "PipelineBuilderVK.h"
#include "DeviceVK.h"
#include "VertexAttributeTypesVK.h"

namespace kuas
{
    static const VkPipelineInputAssemblyStateCreateInfo pointListIA = []() -> const VkPipelineInputAssemblyStateCreateInfo {
        VkPipelineInputAssemblyStateCreateInfo ret{};
        ret.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        ret.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
        return ret;
    }();

    PipelineBuilderVK::PipelineBuilderVK(
        const RenderStateCreateDesc& renderState,
        VkRenderPass renderPass,
        VkPipelineCache pipelineCache,
        DeviceVK* device) :
        m_renderState(renderState),
        m_renderPass(renderPass),
        m_pipelineCache(pipelineCache),
        m_fn(device->getFunc()),
        m_device(device)
    {
        m_dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        m_dynamicStateInfo.dynamicStateCount = KUAS_ARRAY_SIZE(g_dynamicStates);
        m_dynamicStateInfo.pDynamicStates = g_dynamicStates;

        m_viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        m_viewportInfo.viewportCount = 1;
        m_viewportInfo.pViewports = nullptr;
        m_viewportInfo.scissorCount = 1;
        m_viewportInfo.pScissors = nullptr;

        m_rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        m_rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
        m_rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
        m_rasterizationInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        m_rasterizationInfo.lineWidth = 1.0f;

        m_multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        m_multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        m_depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    }
    
    VkPipeline PipelineBuilderVK::buildRect()
    {
        VkDevice vkdevice = m_device->getDevice();
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        VkPipeline ret;
        bool antialias = m_renderState.rasterizationState->antiAliasedLine;

        VkPipelineShaderStageCreateInfo stages[3]{};

        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module = m_device->getShaderModule(ShaderModuleID::Rect_VS);
        stages[0].pName = "main";

        stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        stages[1].module = antialias
            ? m_device->getShaderModule(ShaderModuleID::RectAA_GS)
            : m_device->getShaderModule(ShaderModuleID::Rect_GS);
        stages[1].pName = "main";

        stages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[2].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[2].module = antialias
            ? m_device->getShaderModule(ShaderModuleID::RectAA_PS)
            : m_device->getShaderModule(ShaderModuleID::Rect_PS);
        stages[2].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = KUAS_ARRAY_SIZE(RectVertexVK::bindings);
        vertexInputInfo.pVertexBindingDescriptions = RectVertexVK::bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = KUAS_ARRAY_SIZE(RectVertexVK::attributes);
        vertexInputInfo.pVertexAttributeDescriptions = RectVertexVK::attributes;

        VkPipelineColorBlendAttachmentState blendAtt{};
        blendAtt.blendEnable = VK_TRUE;
        blendAtt.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAtt.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAtt.colorBlendOp = VK_BLEND_OP_ADD;
        blendAtt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAtt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAtt.alphaBlendOp = VK_BLEND_OP_ADD;
        blendAtt.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &blendAtt;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = KUAS_ARRAY_SIZE(stages);
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pointListIA;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &m_viewportInfo;
        pipelineInfo.pRasterizationState = &m_rasterizationInfo;
        pipelineInfo.pMultisampleState = &m_multisampleInfo;
        pipelineInfo.pDepthStencilState = &m_depthStencilInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &m_dynamicStateInfo;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;

        if (KUAS_VULKAN_FAILED(
            m_fn.vkCreateGraphicsPipelines(vkdevice, m_pipelineCache, 1, &pipelineInfo, nullptr, &ret)))
        {
            return nullptr;
        }

        return ret;
    }

    VkPipeline PipelineBuilderVK::buildRoundedRect()
    {
        VkDevice vkdevice = m_device->getDevice();
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        VkPipeline ret;
        bool antialias = m_renderState.rasterizationState->antiAliasedFill;

        VkPipelineShaderStageCreateInfo stages[3]{};

        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module = m_device->getShaderModule(ShaderModuleID::RoundedRect_VS);
        stages[0].pName = "main";

        stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        stages[1].module = antialias
            ? m_device->getShaderModule(ShaderModuleID::RoundedRectAA_GS)
            : m_device->getShaderModule(ShaderModuleID::RoundedRect_GS);
        stages[1].pName = "main";

        stages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[2].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[2].module = antialias
            ? m_device->getShaderModule(ShaderModuleID::RoundedRectAA_PS)
            : m_device->getShaderModule(ShaderModuleID::RoundedRect_PS);
        stages[2].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = KUAS_ARRAY_SIZE(RoundedRectVertexVK::bindings);
        vertexInputInfo.pVertexBindingDescriptions = RoundedRectVertexVK::bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = KUAS_ARRAY_SIZE(RoundedRectVertexVK::attributes);
        vertexInputInfo.pVertexAttributeDescriptions = RoundedRectVertexVK::attributes;

        VkPipelineColorBlendAttachmentState blendAtt{};
        blendAtt.blendEnable = VK_TRUE;
        blendAtt.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAtt.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAtt.colorBlendOp = VK_BLEND_OP_ADD;
        blendAtt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAtt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAtt.alphaBlendOp = VK_BLEND_OP_ADD;
        blendAtt.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &blendAtt;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = KUAS_ARRAY_SIZE(stages);
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pointListIA;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &m_viewportInfo;
        pipelineInfo.pRasterizationState = &m_rasterizationInfo;
        pipelineInfo.pMultisampleState = &m_multisampleInfo;
        pipelineInfo.pDepthStencilState = &m_depthStencilInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &m_dynamicStateInfo;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;

        if (KUAS_VULKAN_FAILED(
            m_fn.vkCreateGraphicsPipelines(vkdevice, m_pipelineCache, 1, &pipelineInfo, nullptr, &ret)))
        {
            return nullptr;
        }

        return ret;
    }

    VkPipeline PipelineBuilderVK::buildFillRect()
    {
        VkDevice vkdevice = m_device->getDevice();
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        VkPipeline ret;
        bool antialias = m_renderState.rasterizationState->antiAliasedFill;

        VkPipelineShaderStageCreateInfo stages[3]{};

        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module = m_device->getShaderModule(ShaderModuleID::FillRect_VS);
        stages[0].pName = "main";

        stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        stages[1].module = antialias
                           ? m_device->getShaderModule(ShaderModuleID::FillRectAA_GS)
                           : m_device->getShaderModule(ShaderModuleID::FillRect_GS);
        stages[1].pName = "main";

        stages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[2].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[2].module = antialias
                           ? m_device->getShaderModule(ShaderModuleID::FillRectAA_PS)
                           : m_device->getShaderModule(ShaderModuleID::FillRect_PS);
        stages[2].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = KUAS_ARRAY_SIZE(FillRectVertexVK::bindings);
        vertexInputInfo.pVertexBindingDescriptions = FillRectVertexVK::bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = KUAS_ARRAY_SIZE(FillRectVertexVK::attributes);
        vertexInputInfo.pVertexAttributeDescriptions = FillRectVertexVK::attributes;

        VkPipelineColorBlendAttachmentState blendAtt{};

        if (antialias) {
            blendAtt.blendEnable = VK_TRUE;
            blendAtt.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
            blendAtt.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            blendAtt.colorBlendOp = VK_BLEND_OP_ADD;
            blendAtt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
            blendAtt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            blendAtt.alphaBlendOp = VK_BLEND_OP_ADD;
        }
        
        blendAtt.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &blendAtt;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = KUAS_ARRAY_SIZE(stages);
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pointListIA;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &m_viewportInfo;
        pipelineInfo.pRasterizationState = &m_rasterizationInfo;
        pipelineInfo.pMultisampleState = &m_multisampleInfo;
        pipelineInfo.pDepthStencilState = &m_depthStencilInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &m_dynamicStateInfo;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;

        if (KUAS_VULKAN_FAILED(
            m_fn.vkCreateGraphicsPipelines(vkdevice, m_pipelineCache, 1, &pipelineInfo, nullptr, &ret)))
        {
            return nullptr;
        }

        return ret;
    }

    VkPipeline PipelineBuilderVK::buildFillRoundedRect()
    {
        VkDevice vkdevice = m_device->getDevice();
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        VkPipeline ret;
        bool antialias = m_renderState.rasterizationState->antiAliasedFill;

        VkPipelineShaderStageCreateInfo stages[3]{};

        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module = m_device->getShaderModule(ShaderModuleID::FillRoundedRect_VS);
        stages[0].pName = "main";

        stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        stages[1].module = antialias
            ? m_device->getShaderModule(ShaderModuleID::FillRoundedRectAA_GS)
            : m_device->getShaderModule(ShaderModuleID::FillRoundedRect_GS);
        stages[1].pName = "main";

        stages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[2].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[2].module = antialias
            ? m_device->getShaderModule(ShaderModuleID::FillRoundedRectAA_PS)
            : m_device->getShaderModule(ShaderModuleID::FillRoundedRect_PS);
        stages[2].pName = "main";

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = KUAS_ARRAY_SIZE(FillRoundedRectVertexVK::bindings);
        vertexInputInfo.pVertexBindingDescriptions = FillRoundedRectVertexVK::bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = KUAS_ARRAY_SIZE(FillRoundedRectVertexVK::attributes);
        vertexInputInfo.pVertexAttributeDescriptions = FillRoundedRectVertexVK::attributes;

        VkPipelineColorBlendAttachmentState blendAtt{};
        blendAtt.blendEnable = VK_TRUE;
        blendAtt.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAtt.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAtt.colorBlendOp = VK_BLEND_OP_ADD;
        blendAtt.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAtt.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAtt.alphaBlendOp = VK_BLEND_OP_ADD;
        blendAtt.colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendInfo{};
        colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendInfo.attachmentCount = 1;
        colorBlendInfo.pAttachments = &blendAtt;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = KUAS_ARRAY_SIZE(stages);
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &pointListIA;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &m_viewportInfo;
        pipelineInfo.pRasterizationState = &m_rasterizationInfo;
        pipelineInfo.pMultisampleState = &m_multisampleInfo;
        pipelineInfo.pDepthStencilState = &m_depthStencilInfo;
        pipelineInfo.pColorBlendState = &colorBlendInfo;
        pipelineInfo.pDynamicState = &m_dynamicStateInfo;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;

        if (KUAS_VULKAN_FAILED(
            m_fn.vkCreateGraphicsPipelines(vkdevice, m_pipelineCache, 1, &pipelineInfo, nullptr, &ret)))
        {
            return nullptr;
        }

        return ret;
    }

    VkPipeline PipelineBuilderVK::buildFillCircle()
    {
        VkDevice vkdevice = m_device->getDevice();
        return nullptr;
    }

    const VkDynamicState PipelineBuilderVK::g_dynamicStates[3] = {
           VK_DYNAMIC_STATE_VIEWPORT,
           VK_DYNAMIC_STATE_SCISSOR,
           VK_DYNAMIC_STATE_BLEND_CONSTANTS,
    };
}
