#pragma once

#include <kuas/Types.h>
#include <kuas/BackendFactoryVK.h>
#include "VulkanHeaders.h"

namespace kuas
{
    class DeviceVK;

    class PipelineBuilderVK
    {
    public:
        PipelineBuilderVK(
            const RenderStateCreateDesc& renderState,
            VkRenderPass renderPass,
            VkPipelineCache pipelineCache,
            DeviceVK* device);

        VkPipeline buildRect();
        VkPipeline buildFillRect();
        VkPipeline buildFillCircle();

    private:
        VkPipelineDynamicStateCreateInfo m_dynamicStateInfo{};
        VkPipelineViewportStateCreateInfo m_viewportInfo{};
        VkPipelineRasterizationStateCreateInfo m_rasterizationInfo{};
        VkPipelineMultisampleStateCreateInfo m_multisampleInfo{};
        VkPipelineDepthStencilStateCreateInfo m_depthStencilInfo{};
        VkRenderPass m_renderPass;
        VkPipelineCache m_pipelineCache;
        const RenderStateCreateDesc& m_renderState;
        const VulkanFunctions& m_fn;
        DeviceVK* m_device;

        static const VkDynamicState g_dynamicStates[3];
    };
}
