#include "CommandDispatcher.h"
#include "PipelineID.h"

#define RESERVE_COMMAND_LIST_SIZE 256

namespace kuas
{
    CommandDispatcher::CommandDispatcher() :
        m_currentPipeline(0),
        m_vtxDrawByteOffset(0),
        m_idxDrawOffset(0)
    {
    }
    
    CommandDispatcher::~CommandDispatcher()
    {
    }

    void CommandDispatcher::begin()
    {
        m_commandList.reset();
        m_vtxDrawByteOffset = 0;
        m_idxDrawOffset = 0;
    }

    void CommandDispatcher::beginDrawPass(
        DrawPass* drawPass,
        RenderTarget* renderTarget,
        const ColorRGBA* clearValue)
    {
        m_commandList.beginDrawPass(drawPass, renderTarget, clearValue);
        DrawList::beginDrawPass(drawPass, renderTarget, clearValue);
    }

    void CommandDispatcher::endDrawPass()
    {
        flushCommands(PipelineID::Unknown); // make sure all commands are completely flushed
        DrawList::endDrawPass();
        m_commandList.endDrawPass();
    }

    void CommandDispatcher::blitImage(Image* srcImage, const Rect2F& srcRect, Image* dstImage, const Rect2F& dstRect)
    {
    }

    void CommandDispatcher::copyImage()
    {
    }

    void CommandDispatcher::copyBitmapToImage()
    {
    }

    void CommandDispatcher::copyImageToBitmap()
    {
    }

    void CommandDispatcher::flushCommands(uint32_t newPipeline)
    {
        if ((m_vtxDrawCount == 0 && m_idxDrawCount == 0)
            || m_currentPipeline == PipelineID::Unknown) {
            m_currentPipeline = newPipeline;
            return;
        }

        if (m_currentRenderState != nullptr
            && m_currentPipeline != newPipeline)
        {
            // Do i need to update viewport?
            if (m_viewportUpdated) {
                m_commandList.setViewport(m_viewport);
                m_viewportUpdated = false;
            }

            // Do i need to update scissor?
            if (m_scissorRectUpdated) {
                m_commandList.setScissorRect(m_scissorRect);
                m_scissorRectUpdated = false;
            }

            if (m_transformMatUpdated) {
                m_commandList.setTransformation(m_transform);
                m_transformMatUpdated = false;
            }

            if (m_idxDrawCount == 0) {
                m_commandList.draw(
                    m_currentRenderState,
                    m_currentPipeline,
                    m_vtxDrawCount,
                    m_vtxDrawByteOffset);

                m_vtxDrawCount = 0;
            }
            else {
                m_commandList.drawIndexed(
                    m_currentRenderState,
                    m_currentPipeline,
                    m_idxDrawCount,
                    m_idxDrawOffset,
                    m_vtxDrawByteOffset);

                m_idxDrawOffset += m_idxOffset;
                m_idxDrawCount = 0;
            }
            
            m_vtxDrawByteOffset += m_vtxDrawSize;
            m_vtxDrawSize = 0;
            m_currentPipeline = newPipeline;
        }
    }
}
