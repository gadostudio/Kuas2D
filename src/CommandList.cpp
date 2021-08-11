#include "CommandList.h"
#include <kuas/DebugAssert.h>
#include <memory>

namespace kuas
{
    CommandList::CommandList() :
        m_cmdList((CommandData*)std::malloc(RESERVE_COMMAND_LIST_SIZE * sizeof(CommandData))),
        m_currentCmd(0),
        m_size(RESERVE_COMMAND_LIST_SIZE)
    {
    }

    CommandList::~CommandList()
    {
        std::free(m_cmdList);
    }

    void CommandList::beginDrawPass(DrawPass* drawPass, RenderTarget* renderTarget, const ColorRGBA* clearValue)
    {
        reserveCommand();

        CommandData& cmd = m_cmdList[m_currentCmd++];
        cmd.type = CommandType::CmdBeginDrawPass;
        cmd.cmdBeginDrawPass.drawPass = drawPass;
        cmd.cmdBeginDrawPass.renderTarget = renderTarget;

        if (clearValue != nullptr) {
            cmd.cmdBeginDrawPass.clearValue.r = clearValue->r;
            cmd.cmdBeginDrawPass.clearValue.g = clearValue->g;
            cmd.cmdBeginDrawPass.clearValue.b = clearValue->b;
            cmd.cmdBeginDrawPass.clearValue.a = clearValue->a;
        }
        else {
            cmd.cmdBeginDrawPass.clearValue.r = 0.0f;
            cmd.cmdBeginDrawPass.clearValue.g = 0.0f;
            cmd.cmdBeginDrawPass.clearValue.b = 0.0f;
            cmd.cmdBeginDrawPass.clearValue.a = 1.0f;
        }
    }

    void CommandList::setViewport(const ViewportDesc& viewport)
    {
        reserveCommand();

        CommandData& cmd = m_cmdList[m_currentCmd++];
        cmd.type = CommandType::CmdSetViewport;
        cmd.cmdSetViewport = viewport;
    }

    void CommandList::setScissorRect(const Rect2I& rect)
    {
        reserveCommand();

        CommandData& cmd = m_cmdList[m_currentCmd++];
        cmd.type = CommandType::CmdSetScissorRect;
        cmd.cmdSetScissorRect = rect;
    }

    void CommandList::setTransformation(const Mat3F& mat)
    {
        reserveCommand();

        CommandData& cmd = m_cmdList[m_currentCmd++];
        cmd.type = CommandType::CmdSetTransformation;
        cmd.cmdSetTransformation = mat;
    }

    void CommandList::draw(
        RenderState* renderState,
        uint32_t pipeline,
        uint32_t numVertices,
        uint32_t vertexByteOffset)
    {
        reserveCommand();

        CommandData& cmd = m_cmdList[m_currentCmd++];
        cmd.type = CommandType::CmdDraw;
        cmd.cmdDraw.pipeline = pipeline;
        cmd.cmdDraw.renderState = renderState;
        cmd.cmdDraw.numVertices = numVertices;
        cmd.cmdDraw.vertexByteOffset = vertexByteOffset;
    }

    void CommandList::drawIndexed(
        RenderState* renderState,
        uint32_t pipeline,
        uint32_t numIndices,
        uint32_t indexOffset,
        uint32_t vertexByteOffset)
    {
        reserveCommand();

        CommandData& cmd = m_cmdList[m_currentCmd++];
        cmd.type = CommandType::CmdDrawIndexed;
        cmd.cmdDrawIndexed.pipeline = pipeline;
        cmd.cmdDrawIndexed.renderState = renderState;
        cmd.cmdDrawIndexed.numIndices = numIndices;
        cmd.cmdDrawIndexed.indexOffset = indexOffset;
        cmd.cmdDrawIndexed.vertexByteOffset = vertexByteOffset;
    }

    void CommandList::endDrawPass()
    {
        reserveCommand();

        CommandData& cmd = m_cmdList[m_currentCmd++];
        cmd.type = CommandType::CmdEndDrawPass;
    }

    void CommandList::reset()
    {
        m_currentCmd = 0;
    }

    void CommandList::reserveCommand()
    {
        if (m_currentCmd > m_size) {
            m_size += RESERVE_COMMAND_LIST_SIZE;
            CommandData* newCmdList = (CommandData*)std::realloc(m_cmdList, m_size * sizeof(CommandData));
            KUAS_ASSERT(newCmdList != nullptr && "Cannot reserve command list memory");
            m_cmdList = newCmdList;
        }
    }
}
