#pragma once

#include <vector>
#include <kuas/Types.h>
#include <kuas/DrawList.h>
#include "CommandList.h"

namespace kuas
{
    class CommandDispatcher : public DrawList
    {
    public:
        CommandDispatcher();
        virtual ~CommandDispatcher();

        void begin() override;

        void beginPaint(
            DrawPass* drawPass,
            Canvas* canvas,
            const ColorRGBA* clearValue) override;

        void endPaint() override;

        void blitImage(Image* srcImage, const Rect2F& srcRect, Image* dstImage, const Rect2F& dstRect) override;
        void copyImage() override;
        void copyBitmapToImage() override;
        void copyImageToBitmap() override;

        void flushCommands(uint32_t newPipeline) override;

    protected:
        CommandList m_commandList;
        uint32_t m_vtxDrawByteOffset;
        uint32_t m_idxDrawOffset;
        uint32_t m_currentPipeline;
    };
}
