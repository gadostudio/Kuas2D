#pragma once

#include "Types.h"
#include "Image.h"
#include "DrawPass.h"
#include "RenderTarget.h"
#include "math/Vec2.h"
#include "math/Mat3.h"
#include "math/ColorRGBA.h"

#include <vector>

namespace kuas
{
    class DrawList : public RefCount
    {
    public:
        virtual ~DrawList();

        virtual void begin();
        
        virtual void beginDrawPass(
            DrawPass* drawPass,
            RenderTarget* renderTarget,
            const ColorRGBA* clearValue);

        void setRenderState(RenderState* renderState);
        void setViewport(const ViewportDesc& viewport);
        void setScissorRect(const Rect2I& rect);
        void setTransformation(const Mat3F& mat);
        void setLineThickness(float thickness);
        void setLineColor(const ColorRGBA& color);
        void setFillColor(const ColorRGBA& color);
        void drawRect(const Rect2F& rect);
        void drawRoundedRect(const Rect2F& rect, float roundness);
        void drawTriangle(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2);
        void drawQuad(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2, const Vec2F& p3);
        void drawCircle(const Vec2F& pos, float radius);
        void fillRect(const Rect2F& rect);
        void fillRoundedRect(const Rect2F& rect, float roundness);
        void fillTriangle(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2);
        void fillQuad(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2, const Vec2F& p3);
        void fillCircle(const Vec2F& pos, float radius);
        virtual void endDrawPass();
        virtual void blitImage(Image* srcImage, const Rect2F& srcRect, Image* dstImage, const Rect2F& dstRect) = 0;
        virtual void copyImage() = 0;
        virtual void copyBitmapToImage() = 0;
        virtual void copyImageToBitmap() = 0;
        virtual void end();

    protected:
        RenderState*    m_currentRenderState;
        ViewportDesc    m_viewport;
        Rect2I          m_scissorRect;
        float           m_lineThickness;
        ColorRGBA       m_lineColor;
        ColorRGBA       m_fillColor;
        Mat3F           m_transform;

        bool            m_viewportUpdated;
        bool            m_scissorRectUpdated;
        bool            m_transformMatUpdated;
        bool            m_insideDrawPass;
        
        void*           m_mappedIdxBuffer;
        void*           m_mappedVtxBuffer;
        uint32_t        m_idxOffset;
        uint32_t        m_idxDrawCount;
        uint32_t        m_totalIdxCount;
        uint32_t        m_vtxCurrentWriteOffset;
        uint32_t        m_vtxBufferSize;
        uint32_t        m_vtxDrawCount;
        uint32_t        m_vtxDrawSize;

        DrawList();

        virtual void flushCommands(uint32_t newPipeline) = 0;
    };
}
