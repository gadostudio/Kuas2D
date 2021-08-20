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
        m_lineThickness = std::max(0.0f, (thickness - 1.0f) * 0.5f);
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

    void DrawList::drawRect(float x, float y, float width, float height)
    {
        drawRect({ x, y, width, height });
    }

    void DrawList::drawRoundedRect(const Rect2F& rect, float roundness)
    {
        flushCommands(PipelineID::RoundedRect);

        RoundedRectVertex* vtx = reinterpret_cast<RoundedRectVertex*>((char*)m_mappedVtxBuffer + m_vtxCurrentWriteOffset);
        vtx->posMin.x = rect.x;
        vtx->posMin.y = rect.y;
        vtx->posMax.x = rect.x + rect.width;
        vtx->posMax.y = rect.y + rect.height;
        vtx->col = m_lineColor;
        vtx->thickness = m_lineThickness;
        vtx->roundness = roundness;

        m_vtxCurrentWriteOffset += sizeof(RoundedRectVertex);
        m_vtxDrawSize += sizeof(RoundedRectVertex);
        m_vtxDrawCount++;
    }

    void DrawList::drawRoundedRect(float x, float y, float width, float height, float roundness)
    {
        drawRoundedRect({ x, y, width, height }, roundness);
    }

    void DrawList::drawCircle(const Vec2F& centerPos, float radius)
    {
        flushCommands(PipelineID::Circle);

        CircleVertex* vtx = reinterpret_cast<CircleVertex*>((char*)m_mappedVtxBuffer + m_vtxCurrentWriteOffset);
        vtx->pos.x = centerPos.x;
        vtx->pos.y = centerPos.y;
        vtx->col = m_lineColor;
        vtx->radius = radius;
        vtx->thickness = m_lineThickness;

        m_vtxCurrentWriteOffset += sizeof(CircleVertex);
        m_vtxDrawSize += sizeof(CircleVertex);
        m_vtxDrawCount++;
    }

    void DrawList::drawCircle(float cx, float cy, float radius)
    {
        drawCircle(Vec2F(cx, cy), radius);
    }

    void DrawList::drawArc(const Vec2F& centerPos, float startAngle, float endAngle, float radius)
    {
    }

    void DrawList::drawArc(float cx, float cy, float startAngle, float endAngle, float radius)
    {
    }

    void DrawList::drawPie(const Vec2F& centerPos, float startAngle, float endAngle, float radius)
    {
    }

    void DrawList::drawPie(float cx, float cy, float startAngle, float endAngle, float radius)
    {
    }

    void DrawList::drawEllipse(const Vec2F& centerPos, float rx, float ry)
    {
    }

    void DrawList::drawEllipse(float cx, float cy, float rx, float ry)
    {
    }

    void DrawList::drawEllipticArc(const Vec2F& centerPos, float startAngle, float endAngle, float rx, float ry)
    {
    }

    void DrawList::drawEllipticArc(float cx, float cy, float startAngle, float endAngle, float rx, float ry)
    {
    }

    void DrawList::drawEllipticPie(const Vec2F& centerPos, float startAngle, float endAngle, float rx, float ry)
    {
    }

    void DrawList::drawEllipticPie(float cx, float cy, float startAngle, float endAngle, float rx, float ry)
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

    void DrawList::fillRect(float x, float y, float width, float height)
    {
        fillRect({ x, y, width, height });
    }

    void DrawList::fillRoundedRect(const Rect2F& rect, float roundness)
    {
        flushCommands(PipelineID::FillRoundedRect);

        FillRoundedRectVertex* vtx = reinterpret_cast<FillRoundedRectVertex*>((char*)m_mappedVtxBuffer + m_vtxCurrentWriteOffset);
        vtx->posMin.x = rect.x;
        vtx->posMin.y = rect.y;
        vtx->posMax.x = rect.x + rect.width;
        vtx->posMax.y = rect.y + rect.height;
        vtx->col = m_fillColor;
        vtx->roundness = roundness;

        m_vtxCurrentWriteOffset += sizeof(FillRoundedRectVertex);
        m_vtxDrawSize += sizeof(FillRoundedRectVertex);
        m_vtxDrawCount++;
    }

    void DrawList::fillRoundedRect(float x, float y, float width, float height, float roundness)
    {
        fillRoundedRect({ x, y, width, height }, roundness);
    }

    void DrawList::fillCircle(const Vec2F& centerPos, float radius)
    {
        flushCommands(PipelineID::FillCircle);

        FillCircleVertex* vtx = reinterpret_cast<FillCircleVertex*>((char*)m_mappedVtxBuffer + m_vtxCurrentWriteOffset);
        vtx->pos.x = centerPos.x;
        vtx->pos.y = centerPos.y;
        vtx->col = m_fillColor;
        vtx->radius = radius;

        m_vtxCurrentWriteOffset += sizeof(FillCircleVertex);
        m_vtxDrawSize += sizeof(FillCircleVertex);
        m_vtxDrawCount++;
    }

    void DrawList::fillCircle(float cx, float cy, float radius)
    {
        fillCircle(Vec2F(cx, cy), radius);
    }

    void DrawList::fillPie(const Vec2F& centerPos, float startAngle, float endAngle, float radius)
    {
    }

    void DrawList::fillPie(float cx, float cy, float startAngle, float endAngle, float radius)
    {
    }

    void DrawList::fillEllipse(const Vec2F& centerPos, float rx, float ry)
    {
    }

    void DrawList::fillEllipse(float cx, float cy, float rx, float ry)
    {
    }

    void DrawList::fillEllipticArc(const Vec2F& centerPos, float startAngle, float endAngle, float rx, float ry)
    {
    }

    void DrawList::fillEllipticArc(float cx, float cy, float startAngle, float endAngle, float rx, float ry)
    {
    }

    void DrawList::fillEllipticPie(const Vec2F& centerPos, float startAngle, float endAngle, float rx, float ry)
    {
    }

    void DrawList::fillEllipticPie(float cx, float cy, float startAngle, float endAngle, float rx, float ry)
    {
    }

    void DrawList::endDrawPass()
    {
        m_insideDrawPass = false;
    }

    void DrawList::end()
    {
    }
}
