#pragma once

#include <kuas/Types.h>
#include <kuas/RefCount.h>
#include <kuas/DrawPass.h>
#include <kuas/Canvas.h>
#include <kuas/RenderState.h>
#include <kuas/math/Mat3.h>

#define RESERVE_COMMAND_LIST_SIZE 256

namespace kuas
{
    enum class CommandType
    {
        CmdBeginPaint,
        CmdSetViewport,
        CmdSetScissorRect,
        CmdSetTransformation,
        CmdDraw,
        CmdDrawIndexed,
        CmdEndPaint,
        CmdCopyImage,
        CmdBlitImage,
        CmdCopyImageToBitmap,
        CmdCopyBitmapToImage
    };

    struct CmdBeginPaintArgs
    {
        DrawPass* drawPass;
        Canvas* canvas;
        ColorRGBA clearValue;
    };

    struct CmdDrawArgs
    {
        RenderState* renderState;
        uint32_t pipeline;
        uint32_t numVertices;
        uint32_t vertexByteOffset;
    };

    struct CmdDrawIndexedArgs
    {
        RenderState* renderState;
        uint32_t pipeline;
        uint32_t numIndices;
        uint32_t indexOffset;
        uint32_t vertexByteOffset;
    };

    struct CommandData
    {
        CommandType type;

        union
        {
            CmdBeginPaintArgs       cmdBeginPaint;
            uint32_t                cmdSetPipeline;
            ViewportDesc            cmdSetViewport;
            Rect2I                  cmdSetScissorRect;
            Mat3F                   cmdSetTransformation;
            CmdDrawArgs             cmdDraw;
            CmdDrawIndexedArgs      cmdDrawIndexed;
        };
    };

    struct CommandListIterator
    {
        CommandData* ptr;

        CommandListIterator(CommandData* ptr) :
            ptr(ptr)
        {
        }

        CommandData& operator*()
        {
            return *ptr;
        }

        CommandListIterator& operator++()
        {
            ++ptr;
            return *this;
        }

        CommandListIterator operator++(int)
        {
            CommandListIterator tmp(ptr);
            ptr++;
            return tmp;
        }

        bool operator!=(const CommandListIterator& other)
        {
            return ptr != other.ptr;
        }
    };

    class CommandList
    {
    public:
        CommandList();
        ~CommandList();

        void beginPaint(
            DrawPass* drawPass,
            Canvas* canvas,
            const ColorRGBA* clearValue);

        void setViewport(const ViewportDesc& viewport);

        void setScissorRect(const Rect2I& rect);

        void setTransformation(const Mat3F& mat);

        void draw(
            RenderState* renderState,
            uint32_t pipeline,
            uint32_t numVertices,
            uint32_t vertexByteOffset);

        void drawIndexed(
            RenderState* renderState,
            uint32_t pipeline,
            uint32_t numIndices,
            uint32_t indexOffset,
            uint32_t vertexByteOffset);

        void endPaint();

        void reset();

        CommandListIterator begin() { return CommandListIterator(&m_cmdList[0]); }
        CommandListIterator end() { return CommandListIterator(&m_cmdList[m_currentCmd]); }

    private:
        CommandData* m_cmdList;
        uint32_t m_currentCmd;
        uint32_t m_size;

        void reserveCommand();
    };
}
