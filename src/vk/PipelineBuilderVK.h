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
        VkPipeline buildRoundedRect();
        VkPipeline buildFillRect();
        VkPipeline buildFillRoundedRect();
        VkPipeline buildCircle();
        VkPipeline buildFillCircle();
        VkPipeline buildFillEllipse();

    private:
        VkRenderPass m_renderPass;
        VkPipelineCache m_pipelineCache;
        const RenderStateCreateDesc& m_renderState;
        const VulkanFunctions& m_fn;
        DeviceVK* m_device;


        VkPipeline buildPipeline(
            VkShaderModule vs,
            VkShaderModule gs,
            VkShaderModule fs,
            VkPipelineLayout layout,
            const VkPipelineInputAssemblyStateCreateInfo& inputAssembly,
            const VkPipelineVertexInputStateCreateInfo& vertexInput,
            const VkPipelineColorBlendStateCreateInfo& colorBlend);
    };
}
