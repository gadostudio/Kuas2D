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
    }
    
    VkPipeline PipelineBuilderVK::buildRect()
    {
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        bool antialias = m_renderState.rasterizationState->antiAliasedLine;

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

        return buildPipeline(
            m_device->getShaderModule(ShaderModuleID::Rect_VS),
            m_device->getShaderModule(antialias ? ShaderModuleID::RectAA_GS : ShaderModuleID::Rect_GS),
            m_device->getShaderModule(antialias ? ShaderModuleID::RectAA_PS : ShaderModuleID::Rect_PS),
            layout,
            pointListIA,
            vertexInputInfo,
            colorBlendInfo);
    }

    VkPipeline PipelineBuilderVK::buildRoundedRect()
    {
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        bool antialias = m_renderState.rasterizationState->antiAliasedLine;

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

        return buildPipeline(
            m_device->getShaderModule(ShaderModuleID::RoundedRect_VS),
            m_device->getShaderModule(antialias ? ShaderModuleID::RoundedRectAA_GS : ShaderModuleID::RoundedRect_GS),
            m_device->getShaderModule(antialias ? ShaderModuleID::RoundedRectAA_PS : ShaderModuleID::RoundedRect_PS),
            layout,
            pointListIA,
            vertexInputInfo,
            colorBlendInfo);
    }

    VkPipeline PipelineBuilderVK::buildFillRect()
    {
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        bool antialias = m_renderState.rasterizationState->antiAliasedFill;

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

        return buildPipeline(
            m_device->getShaderModule(ShaderModuleID::FillRect_VS),
            m_device->getShaderModule(antialias ? ShaderModuleID::FillRectAA_GS : ShaderModuleID::FillRect_GS),
            m_device->getShaderModule(antialias ? ShaderModuleID::FillRectAA_PS : ShaderModuleID::FillRect_PS),
            layout,
            pointListIA,
            vertexInputInfo,
            colorBlendInfo);
    }

    VkPipeline PipelineBuilderVK::buildFillRoundedRect()
    {
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        bool antialias = m_renderState.rasterizationState->antiAliasedFill;

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

        return buildPipeline(
            m_device->getShaderModule(ShaderModuleID::FillRoundedRect_VS),
            m_device->getShaderModule(antialias ? ShaderModuleID::FillRoundedRectAA_GS : ShaderModuleID::FillRoundedRect_GS),
            m_device->getShaderModule(antialias ? ShaderModuleID::FillRoundedRectAA_PS : ShaderModuleID::FillRoundedRect_PS),
            layout,
            pointListIA,
            vertexInputInfo,
            colorBlendInfo);
    }

    VkPipeline PipelineBuilderVK::buildCircle()
    {
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        bool antialias = m_renderState.rasterizationState->antiAliasedLine;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = KUAS_ARRAY_SIZE(CircleVertexVK::bindings);
        vertexInputInfo.pVertexBindingDescriptions = CircleVertexVK::bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = KUAS_ARRAY_SIZE(CircleVertexVK::attributes);
        vertexInputInfo.pVertexAttributeDescriptions = CircleVertexVK::attributes;

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

        return buildPipeline(
            m_device->getShaderModule(ShaderModuleID::Circle_VS),
            m_device->getShaderModule(antialias ? ShaderModuleID::CircleAA_GS : ShaderModuleID::Circle_GS),
            m_device->getShaderModule(antialias ? ShaderModuleID::CircleAA_PS : ShaderModuleID::Circle_PS),
            layout,
            pointListIA,
            vertexInputInfo,
            colorBlendInfo);
    }

    VkPipeline PipelineBuilderVK::buildFillCircle()
    {
        VkPipelineLayout layout = m_device->getCommonPipelineLayout();
        bool antialias = m_renderState.rasterizationState->antiAliasedFill;

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = KUAS_ARRAY_SIZE(FillCircleVertexVK::bindings);
        vertexInputInfo.pVertexBindingDescriptions = FillCircleVertexVK::bindings;
        vertexInputInfo.vertexAttributeDescriptionCount = KUAS_ARRAY_SIZE(FillCircleVertexVK::attributes);
        vertexInputInfo.pVertexAttributeDescriptions = FillCircleVertexVK::attributes;

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

        return buildPipeline(
            m_device->getShaderModule(ShaderModuleID::FillCircle_VS),
            m_device->getShaderModule(antialias ? ShaderModuleID::FillCircleAA_GS : ShaderModuleID::FillCircle_GS),
            m_device->getShaderModule(antialias ? ShaderModuleID::FillCircleAA_PS : ShaderModuleID::FillCircle_PS),
            layout,
            pointListIA,
            vertexInputInfo,
            colorBlendInfo);
    }

    VkPipeline PipelineBuilderVK::buildPipeline(
        VkShaderModule vs,
        VkShaderModule gs,
        VkShaderModule fs,
        VkPipelineLayout layout,
        const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
        const VkPipelineVertexInputStateCreateInfo& vertexInput,
        const VkPipelineColorBlendStateCreateInfo& colorBlend)
    {
        static const VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_BLEND_CONSTANTS
        };

        static const auto dynamicStateInfo = [&]() -> const VkPipelineDynamicStateCreateInfo {
            VkPipelineDynamicStateCreateInfo ret{};
            ret.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
            ret.dynamicStateCount = KUAS_ARRAY_SIZE(dynamicStates);
            ret.pDynamicStates = dynamicStates;
            return ret;
        }();

        static const auto viewportInfo = [&]() -> const VkPipelineViewportStateCreateInfo {
            VkPipelineViewportStateCreateInfo ret{};
            ret.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            ret.viewportCount = 1;
            ret.pViewports = nullptr;
            ret.scissorCount = 1;
            ret.pScissors = nullptr;
            return ret;
        }();

        static const auto rasterizationInfo = [&]() -> const VkPipelineRasterizationStateCreateInfo {
            VkPipelineRasterizationStateCreateInfo ret{};
            ret.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            ret.polygonMode = VK_POLYGON_MODE_FILL;
            ret.cullMode = VK_CULL_MODE_NONE;
            ret.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            ret.lineWidth = 1.0f;
            return ret;
        }();

        static const auto multisampleInfo = [&]() -> const VkPipelineMultisampleStateCreateInfo {
            VkPipelineMultisampleStateCreateInfo ret{};
            ret.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            ret.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            return ret;
        }();

        VkPipelineShaderStageCreateInfo stages[3]{};

        stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        stages[0].module = vs;
        stages[0].pName = "main";

        stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[1].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
        stages[1].module = gs;
        stages[1].pName = "main";

        stages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stages[2].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        stages[2].module = fs;
        stages[2].pName = "main";

        VkPipelineDepthStencilStateCreateInfo depthStencilInfo{};
        depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = KUAS_ARRAY_SIZE(stages);
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pTessellationState = nullptr;
        pipelineInfo.pViewportState = &viewportInfo;
        pipelineInfo.pRasterizationState = &rasterizationInfo;
        pipelineInfo.pMultisampleState = &multisampleInfo;
        pipelineInfo.pDepthStencilState = &depthStencilInfo;
        pipelineInfo.pColorBlendState = &colorBlend;
        pipelineInfo.pDynamicState = &dynamicStateInfo;
        pipelineInfo.layout = layout;
        pipelineInfo.renderPass = m_renderPass;
        pipelineInfo.subpass = 0;

        VkDevice vkdevice = m_device->getDevice();
        VkPipeline pipeline;

        if (KUAS_VULKAN_FAILED(
            m_fn.vkCreateGraphicsPipelines(vkdevice, m_pipelineCache, 1, &pipelineInfo, nullptr, &pipeline)))
        {
            return nullptr;
        }

        return pipeline;
    }
}
