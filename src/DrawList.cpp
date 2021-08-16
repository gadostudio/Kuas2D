#include <kuas/DrawList.h>
#include <algorithm>
#include "PipelineID.h"
#include "VertexAttributeTypes.h"

namespace kuas
{
    DrawList::DrawList() :
        m_currentRenderState(nullptr),
        m_viewport(),
        m_scissorRect(),
        m_transform(),
        m_lineThickness(0.0f),
        m_lineColor(0.0f),
        m_fillColor(0.0f),
        m_viewportUpdated(false),
        m_scissorRectUpdated(false),
        m_transformMatUpdated(true),
        m_insideDrawPass(false),
        m_mappedIdxBuffer(nullptr),
        m_mappedVtxBuffer(nullptr),
        m_idxOffset(0),
        m_idxDrawCount(0),
        m_totalIdxCount(0),
        m_vtxCurrentWriteOffset(0),
        m_vtxBufferSize(0),
        m_vtxDrawCount(0),
        m_vtxDrawSize(0)
    {
    }

    DrawList::~DrawList()
    {
    }

    void DrawList::begin()
    {
    }

    void DrawList::beginDrawPass(
        DrawPass* drawPass, RenderTarget* renderTarget,
        const ColorRGBA* clearValue)
    {
        m_insideDrawPass = true;
    }

    void DrawList::setRenderState(RenderState* renderState)
    {
        m_currentRenderState = renderState;
    }

    void DrawList::setViewport(const ViewportDesc& viewport)
    {
        m_viewport = viewport;
        m_viewportUpdated = true;
    }

    void DrawList::setScissorRect(const Rect2I& rect)
    {
        m_scissorRect = rect;
        m_scissorRectUpdated = true;
    }

    void DrawList::setTransformation(const Mat3F& mat)
    {
        flushCommands(PipelineID::Unknown);
        m_transform = mat;
        m_transformMatUpdated = true;
    }

    void DrawList::setLineThickness(float thickness)
    {
        m_lineThickness = std::max(0.0f, thickness * 0.5f);
    }

    void DrawList::setLineColor(const ColorRGBA& color)
    {
        m_lineColor = color;
    }

    void DrawList::setFillColor(const ColorRGBA& color)
    {
        m_fillColor = color;
    }

    void DrawList::drawRect(const Rect2F& rect)
    {
        flushCommands(PipelineID::Rect);

        RectVertex* vtx = reinterpret_cast<RectVertex*>((char*)m_mappedVtxBuffer + m_vtxCurrentWriteOffset);

        vtx->posMin.x = rect.x;
        vtx->posMin.y = rect.y;
        vtx->posMax.x = rect.x + rect.width;
        vtx->posMax.y = rect.y + rect.height;
        vtx->col = m_lineColor;
        vtx->thickness = m_lineThickness;

        m_vtxCurrentWriteOffset += sizeof(RectVertex);
        m_vtxDrawSize += sizeof(RectVertex);
        m_vtxDrawCount++;
    }

    void DrawList::drawRoundedRect(const Rect2F& rect, float roundness)
    {    
    }    

    void DrawList::drawTriangle(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2)
    {    
    }    

    void DrawList::drawQuad(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2, const Vec2F& p3)
    {    
    }    

    void DrawList::drawCircle(const Vec2F& pos, float radius)
    {    
    }    

    void DrawList::fillRect(const Rect2F& rect)
    {
        flushCommands(PipelineID::FillRect);

        FillRectVertex* vtx = reinterpret_cast<FillRectVertex*>((char*)m_mappedVtxBuffer + m_vtxCurrentWriteOffset);

        vtx->posMin.x = rect.x;
        vtx->posMin.y = rect.y;
        vtx->posMax.x = rect.x + rect.width;
        vtx->posMax.y = rect.y + rect.height;
        vtx->col = m_fillColor;

        m_vtxCurrentWriteOffset += sizeof(FillRectVertex);
        m_vtxDrawSize += sizeof(FillRectVertex);
        m_vtxDrawCount++;
    }

    void DrawList::fillRoundedRect(const Rect2F& rect, float roundness)
    {
        flushCommands(PipelineID::FillRoundedRect);
    }

    void DrawList::fillTriangle(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2)
    {
        flushCommands(PipelineID::FillTriangle);
    }

    void DrawList::fillQuad(const Vec2F& p0, const Vec2F& p1, const Vec2F& p2, const Vec2F& p3)
    {
    }

    void DrawList::fillCircle(const Vec2F& pos, float radius)
    {
        flushCommands(PipelineID::FillCircle);
    }

    void DrawList::endDrawPass()
    {
        m_insideDrawPass = false;
    }

    void DrawList::end()
    {
    }
}
