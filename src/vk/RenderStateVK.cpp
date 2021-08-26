#include "RenderStateVK.h"
#include "DeviceVK.h"

namespace kuas
{
    RenderStateVK::RenderStateVK(
        VkPipeline rectPipe,
        VkPipeline roundedRectPipe,
        VkPipeline circlePipe,
        VkPipeline fillRectPipe,
        VkPipeline fillRoundedRectPipe,
        VkPipeline fillCirclePipe,
        VkPipeline fillEllipsePipe,
        VkPipeline lineSegmentPipe,
        const ColorStateDesc& colorState,
        DeviceVK* parentDevice) :
        m_pipelines{},
        m_parentDevice(safeAddRef(parentDevice))
    {
        m_pipelines[PipelineID::Unknown] = VK_NULL_HANDLE;
        m_pipelines[PipelineID::Rect] = rectPipe;
        m_pipelines[PipelineID::RoundedRect] = roundedRectPipe;
        m_pipelines[PipelineID::Circle] = circlePipe;
        m_pipelines[PipelineID::FillRect] = fillRectPipe;
        m_pipelines[PipelineID::FillRoundedRect] = fillRoundedRectPipe;
        m_pipelines[PipelineID::FillCircle] = fillCirclePipe;
        m_pipelines[PipelineID::FillEllipse] = fillEllipsePipe;
        m_pipelines[PipelineID::LineSegment] = lineSegmentPipe;
    }

    RenderStateVK::~RenderStateVK()
    {
        const VulkanFunctions& fn = m_parentDevice->getFunc();
        VkDevice device = m_parentDevice->getDevice();
        
        for (auto pipeline : m_pipelines) {
            if (pipeline != nullptr) {
                fn.vkDestroyPipeline(device, pipeline, nullptr);
            }
        }

        m_parentDevice->release();
    }
}
