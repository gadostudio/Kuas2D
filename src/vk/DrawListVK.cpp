#include <kuas/DebugAssert.h>
#include "DrawListVK.h"
#include "DeviceVK.h"
#include "DrawPassVK.h"
#include "RenderTargetVK.h"
#include "RenderStateVK.h"
#include "DrawParamVK.h"
#include "../PipelineID.h"

#define RESERVE_BUFFER_BYTE_SIZE 32768

namespace kuas
{
    DrawListVK::DrawListVK(VkCommandPool cmdPool, DeviceVK* parentDevice) :
        m_cmdPool(cmdPool),
        m_cmdBuffer(nullptr),
        m_idxBuffer(nullptr),
        m_idxBufferAlloc(nullptr),
        m_idxBufferSize(0),
        m_vtxBuffer(nullptr),
        m_vtxBufferAlloc(nullptr),
        m_vtxBufferSize(0),
        m_allocator(parentDevice->getAllocator()),
        m_fn(parentDevice->getFunc()),
        m_parentDevice(parentDevice)
    {
        init();
    }

    DrawListVK::~DrawListVK()
    {
        destroy();
    }

    void DrawListVK::begin()
    {
        CommandDispatcher::begin();
        vmaMapMemory(m_allocator, m_vtxBufferAlloc, &m_mappedVtxBuffer);
        vmaMapMemory(m_allocator, m_idxBufferAlloc, &m_mappedIdxBuffer);
    }

    void DrawListVK::end()
    {
        const VmaAllocation allocations[2] = {
            m_idxBufferAlloc,
            m_vtxBufferAlloc,
        };

        vmaFlushAllocations(m_allocator, 2, allocations, nullptr, nullptr);
        vmaInvalidateAllocations(m_allocator, 2, allocations, nullptr, nullptr);
        vmaUnmapMemory(m_allocator, allocations[0]);
        vmaUnmapMemory(m_allocator, allocations[1]);

        VkPipeline currentPipeline = VK_NULL_HANDLE;
        VkPipelineLayout layout = m_parentDevice->getCommonPipelineLayout();
        static const VkShaderStageFlags pushConstantStages =
            VK_SHADER_STAGE_VERTEX_BIT |
            VK_SHADER_STAGE_FRAGMENT_BIT |
            VK_SHADER_STAGE_GEOMETRY_BIT;

        // Record command buffer at the end of the draw list to be able to resize vertex/index buffer
        // conveniently. Unfortunately, extra data must be stored and a loop required to
        // iterate & translate draw commands into actual Vulkan commands.
        
        VkCommandBuffer cmdBuffer = m_cmdBuffer;
        VkCommandBufferBeginInfo cmdBegin{};
        cmdBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmdBegin.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        m_fn.vkBeginCommandBuffer(cmdBuffer, &cmdBegin);
        m_fn.vkCmdBindIndexBuffer(cmdBuffer, m_idxBuffer, 0, VK_INDEX_TYPE_UINT32);

        for (const auto& cmd : m_commandList) {
            switch (cmd.type) {
                case CommandType::CmdBeginDrawPass: {
                    const CmdBeginDrawPassArgs& beginDrawPass = cmd.cmdBeginDrawPass;

                    VkClearValue clearColor{};
                    clearColor.color.float32[0] = beginDrawPass.clearValue.r;
                    clearColor.color.float32[1] = beginDrawPass.clearValue.g;
                    clearColor.color.float32[2] = beginDrawPass.clearValue.b;
                    clearColor.color.float32[3] = beginDrawPass.clearValue.a;

                    VkRenderPassBeginInfo rpBegin{};
                    rpBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                    rpBegin.renderPass = KUAS_PTR_CAST(DrawPassVK, beginDrawPass.drawPass)->getHandle();
                    rpBegin.framebuffer = KUAS_PTR_CAST(RenderTargetVK, beginDrawPass.renderTarget)->getHandle();
                    rpBegin.renderArea.offset.x = 0;
                    rpBegin.renderArea.offset.y = 0;
                    rpBegin.renderArea.extent.width = beginDrawPass.renderTarget->getWidth();
                    rpBegin.renderArea.extent.height = beginDrawPass.renderTarget->getHeight();
                    rpBegin.clearValueCount = 1;
                    rpBegin.pClearValues = &clearColor;

                    Vec2F screenSize(
                        2.f / (float)rpBegin.renderArea.extent.width,
                        2.f / (float)rpBegin.renderArea.extent.height);

                    m_fn.vkCmdBeginRenderPass(cmdBuffer, &rpBegin, VK_SUBPASS_CONTENTS_INLINE);
                    m_fn.vkCmdPushConstants(
                        cmdBuffer, layout, pushConstantStages,
                        sizeof(DrawParamVK::m_transformMat),
                        sizeof(DrawParamVK::m_screenSize),
                        &screenSize);

                    break;
                }
                case CommandType::CmdSetViewport: {
                    const ViewportDesc& viewportDesc = cmd.cmdSetViewport;
                    VkViewport viewport{};

                    // negative viewport
                    viewport.x = viewportDesc.x;
                    viewport.y = viewportDesc.y;
                    viewport.width = viewportDesc.width;
                    viewport.height = viewportDesc.height;
                    viewport.minDepth = 0.0f;
                    viewport.maxDepth = 1.0f;

                    Vec2F screenSize(
                        2.f / viewport.width,
                        2.f / viewport.height);

                    m_fn.vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
                    m_fn.vkCmdPushConstants(
                        cmdBuffer, layout, pushConstantStages,
                        sizeof(DrawParamVK::m_transformMat),
                        sizeof(DrawParamVK::m_screenSize),
                        &screenSize);

                    break;
                }
                case CommandType::CmdSetScissorRect: {
                    const Rect2I& scissorRect = cmd.cmdSetScissorRect;
                    VkRect2D rect{};

                    rect.offset.x = scissorRect.x;
                    rect.offset.y = scissorRect.y;
                    rect.extent.width = scissorRect.width;
                    rect.extent.height = scissorRect.height;

                    m_fn.vkCmdSetScissor(cmdBuffer, 0, 1, &rect);
                    break;
                }
                case CommandType::CmdSetTransformation: {
                    const Mat3F& tf = cmd.cmdSetTransformation;

                    // We do not take Mat3F directly because of alignment issue.
                    // This can be optimized
                    DrawParamVK drawParam{};
                    drawParam.m_transformMat[0].r0 = tf.m[0][0];
                    drawParam.m_transformMat[0].r1 = tf.m[0][1];
                    drawParam.m_transformMat[0].r2 = tf.m[0][2];
                    drawParam.m_transformMat[1].r0 = tf.m[1][0];
                    drawParam.m_transformMat[1].r1 = tf.m[1][1];
                    drawParam.m_transformMat[1].r2 = tf.m[1][2];
                    drawParam.m_transformMat[2].r0 = tf.m[2][0];
                    drawParam.m_transformMat[2].r1 = tf.m[2][1];
                    drawParam.m_transformMat[2].r2 = tf.m[2][2];

                    m_fn.vkCmdPushConstants(
                        cmdBuffer, layout,
                        pushConstantStages,
                        0, sizeof(DrawParamVK::m_transformMat),
                        &drawParam);

                    break;
                }
                case CommandType::CmdDraw: {
                    const CmdDrawArgs& draw = cmd.cmdDraw;
                    VkDeviceSize vertexOffset = (VkDeviceSize)draw.vertexByteOffset;
                    VkPipeline newPipeline =
                        KUAS_PTR_CAST(RenderStateVK, draw.renderState)->m_pipelines[draw.pipeline];

                    if (newPipeline != currentPipeline) {
                        m_fn.vkCmdBindPipeline(
                            cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            newPipeline);

                        currentPipeline = newPipeline;
                    }

                    m_fn.vkCmdBindVertexBuffers(
                        cmdBuffer, 0, 1,
                        &m_vtxBuffer,
                        &vertexOffset);

                    m_fn.vkCmdDraw(cmdBuffer, draw.numVertices, 1, 0, 0);

                    break;
                }
                case CommandType::CmdDrawIndexed: {
                    const CmdDrawIndexedArgs& draw = cmd.cmdDrawIndexed;
                    VkDeviceSize vertexOffset = (VkDeviceSize)draw.vertexByteOffset;
                    VkPipeline newPipeline =
                        KUAS_PTR_CAST(RenderStateVK, draw.renderState)->m_pipelines[draw.pipeline];

                    if (newPipeline != currentPipeline) {
                        m_fn.vkCmdBindPipeline(
                            cmdBuffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            newPipeline);

                        currentPipeline = newPipeline;
                    }

                    m_fn.vkCmdBindVertexBuffers(
                        cmdBuffer, 0, 1,
                        &m_vtxBuffer,
                        &vertexOffset);

                    m_fn.vkCmdDrawIndexed(
                        cmdBuffer,
                        draw.numIndices, 1,
                        draw.indexOffset, 0, 0);

                    break;
                }
                case CommandType::CmdEndDrawPass: {
                    m_fn.vkCmdEndRenderPass(cmdBuffer);
                    break;
                }
            }
        }

        m_fn.vkEndCommandBuffer(cmdBuffer);
    }

    void DrawListVK::init()
    {
        VkDevice device = m_parentDevice->getDevice();
        VkCommandBufferAllocateInfo cbAlloc{};

        cbAlloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cbAlloc.commandPool = m_cmdPool;
        cbAlloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        cbAlloc.commandBufferCount = 1;

        m_fn.vkAllocateCommandBuffers(device, &cbAlloc, &m_cmdBuffer);

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = RESERVE_BUFFER_BYTE_SIZE;
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        allocInfo.preferredFlags =
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT |
            VK_MEMORY_PROPERTY_HOST_CACHED_BIT;

        if (KUAS_VULKAN_FAILED(vmaCreateBuffer(
            m_allocator, &bufferInfo, &allocInfo, &m_vtxBuffer, &m_vtxBufferAlloc, nullptr)))
        {
            KUAS_ASSERT(false && "Failed to create vertex buffer");
        }

        bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

        if (KUAS_VULKAN_FAILED(vmaCreateBuffer(
            m_allocator, &bufferInfo, &allocInfo, &m_idxBuffer, &m_idxBufferAlloc, nullptr)))
        {
            KUAS_ASSERT(false && "Failed to create index buffer");
        }
    }

    void DrawListVK::destroy()
    {
        VkDevice device = m_parentDevice->getDevice();

        vmaDestroyBuffer(m_allocator, m_vtxBuffer, m_vtxBufferAlloc);
        vmaDestroyBuffer(m_allocator, m_idxBuffer, m_idxBufferAlloc);
        
        m_fn.vkDestroyCommandPool(device, m_cmdPool, nullptr);
    }

    void DrawListVK::reserveBuffer(uint32_t i)
    {
    }
}
