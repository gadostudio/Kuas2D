#pragma once

#include "ColorRGBA.h"

namespace kuas
{
    // List of supported color space conversion:
    // - CMYK -> RGB
    // - HSL -> RGB
    // - HSV -> RGB
    // - HCL -> RGB
    // - RGB* -> YUV BT.470
    // - RGB* -> YUV BT.709
    // - RGB* -> YUV BT.2020
    // - RGB* -> YUV BT.2100
    // - YUV BT.470 -> RGB*
    // - YUV BT.709 -> RGB*
    // - YUV BT.2020 -> RGB*
    // - YUV BT.2100 -> RGB*
    //
    // (*) gamma-corrected

    struct ColorYUV
    {
        float y, u, v;
    };

    struct ColorYCbCr
    {
        float y, cb, cr;
    };

    // RGB -> Luma (standard BT.407)
    static inline float rgbaToLuma407(const ColorRGBA& c)
    {
        return (0.299f * c.r) + (0.587f * c.g) + (0.114f * c.r);
    }

    // RGB -> Luma (standard BT.709)
    static inline float rgbaToLuma709(const ColorRGBA& c)
    {
        return (0.2126f * c.r) + (0.7152f * c.g) + (0.0722f * c.b);
    }

    static inline ColorYUV rgbaToYUV407(const ColorRGBA& c)
    {
        float y = (0.299f * c.r) + (0.587f * c.g) + (0.114f * c.r);
        return { y, 0.493f * (c.b - y), 0.877f * (c.r - y) };
    }

    static inline ColorYUV rgbaToYUV709(const ColorRGBA& c)
    {
        return { 
            ( 0.2126f  * c.r) + (0.7152f   * c.g) + (0.0722f  * c.b),
            (-0.09991f * c.r) - (0.33609f  * c.g) + (0.436f   * c.b),
            ( 0.615f   * c.r) - (0.55861f  * c.g) - (0.05639f * c.b)
        };
    }

    static inline ColorYUV rgbaToYIQ407(const ColorRGBA& c)
    {
        float y = (0.299f * c.r) + (0.587f * c.g) + (0.114f * c.r);
        return {
             y, 
            -0.27f * (c.b - y) + 0.74f * (c.r - y),
             0.41f * (c.b - y) + 0.48f * (c.r - y)
        };
    }

    static inline ColorYCbCr rgbaToYCbBr601(const ColorRGBA& c)
    {
        return {
            ( 0.299f * c.r) + (0.587f * c.g) + (0.114f * c.b),
            ( 0.701f * c.r) - (0.587f * c.g) - (0.114f * c.b),
            (-0.299f * c.r) - (0.587f * c.g) + (0.886f * c.b),
        };
    }

    static inline ColorRGBA rgbaFromYUV407(const ColorYUV& c, float alpha = 1.0f)
    {
        return ColorRGBA(
            c.y + (1.13983f * c.y),
            c.u - (0.39465f * c.u) - (0.58060f * c.u),
            c.v + (2.03211f * c.v),
            alpha);
    }

    static inline ColorRGBA rgbaFromYUV709(const ColorYUV& c, float alpha = 1.0f)
    {
        return ColorRGBA(
            c.y + (1.28033f * c.y),
            c.u - (0.21482f * c.u) - (0.38059f * c.u),
            c.v + (2.12798f * c.v),
            alpha);
    }
}
