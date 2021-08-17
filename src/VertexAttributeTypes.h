#pragma once

#include <kuas/math/Vec2.h>
#include <kuas/math/ColorRGBA.h>

namespace kuas
{
    struct BasicPolyFillVertex
    {
        Vec2F pos;
        ColorRGBA col;
    };

    struct RectVertex
    {
        Vec2F posMin;
        Vec2F posMax;
        ColorRGBA col;
        float thickness;
    };

    struct RoundedRectVertex
    {
        Vec2F posMin;
        Vec2F posMax;
        ColorRGBA col;
        float thickness;
        float roundness;
    };

    struct FillRectVertex
    {
        Vec2F posMin;
        Vec2F posMax;
        ColorRGBA col;
    };

    struct FillRoundedRectVertex
    {
        Vec2F posMin;
        Vec2F posMax;
        ColorRGBA col;
        float roundness;
    };

    struct CircleVertex
    {
        Vec2F pos;
        ColorRGBA col;
        float radius;
        float thickness;
    };

    struct FillCircleVertex
    {
        Vec2F pos;
        ColorRGBA col;
        float radius;
    };
}
