#pragma once

#include <array>
#include <kuas/RenderState.h>
#include "VulkanHeaders.h"
#include "../PipelineID.h"

namespace kuas
{
    class DeviceVK;
    class DrawPassVK;

    class RenderStateVK : public RenderState
    {
    public:
        RenderStateVK(
            VkPipeline rectPipe,
            VkPipeline roundedRectPipe,
            VkPipeline circlePipe,
            VkPipeline fillRectPipe,
            VkPipeline fillRoundedRectPipe,
            VkPipeline fillCirclePipe,
            const ColorStateDesc& colorState,
            DeviceVK* parentDevice);

        ~RenderStateVK();

        friend class DrawListVK;

    private:
        std::array<VkPipeline, PipelineID::count> m_pipelines;
        DeviceVK* m_parentDevice;
    };
}
