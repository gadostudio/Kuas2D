#pragma once

#include <cstdint>
#include "math/ColorRGBA.h"
#include "math/CommonMath.h"
#include "math/Mat3.h"
#include "math/Vec2.h"

#define KUAS_ARRAY_SIZE(x) \
    (sizeof(x)/sizeof(x[0]))

namespace kuas
{
    // forward declaration
    class Device;
    class Bitmap;
    class Image;
    class DrawPass;
    class RenderTarget;
    class RenderState;
    class DrawList;
    class Semaphore;
    class Fence;

    using BitmapUsageFlags = uint32_t;
    using ImageUsageFlags = uint32_t;
    
    enum class Result
    {
        WaitTimeout,
        Ok,
        ErrOutOfMemory = -1,
        ErrInvalidArg = -2,
        ErrFormatNotSupported = -3,
        ErrDeviceLost = -4,
        ErrUnknown = -5
    };

    enum class PixelFormat
    {
        Undefined,
        R8_Unorm,
        R8_Srgb,
        R4G4_Unorm,
        R5G6B5_Unorm,
        R4G4B4A4_Unorm,
        R5G5B5A1_Unorm,
        A8B8G8R8_Unorm,
        A8B8G8R8_Srgb,
        B8G8R8A8_Unorm,
        B8G8R8A8_Srgb,
        R8G8B8A8_Unorm,
        R8G8B8A8_Srgb,
        A2R10G10B10_Unorm,
        A2B10G10R10_Unorm,
        R16G16_Float,
        R16G16_Unorm,
        R16G16B16A16_Float,
        R16G16B16A16_Unorm,
        R32G32_Float,
        R32G32B32A32_Float,
    };

    struct BitmapUsage
    {
        enum
        {
            Drawing         = 1 << 0,
            Upload          = 1 << 1,
            Readback        = 1 << 2
        };
    };

    struct ImageUsage
    {
        enum
        {
            Drawing             = 1 << 0,
            RenderTargetOutput  = 1 << 1,
            RenderTargetInput   = 1 << 2,
            TransferSrc         = 1 << 3,
            TransferDst         = 1 << 4
        };
    };

    enum class AttachmentOp
    {
        Load,
        Store,
        Clear,
        DontCare
    };

    enum class ColorMode
    {
        Solid,
        Gradient
    };

    enum class GradientMode
    {
        Ramp,
        Radial,
        Conical
    };

    enum class GradientWrapping
    {
        None,
        Repeat,
        Mirror
    };

    enum class InterpolationOp
    {
        Linear,
        Quadratic,
        Cubic
    };

    enum class LineJoin
    {
        Detached,
        Miter,
        Rounded,
        Bevel
    };

    enum class LineCap
    {
        None,
        Rounded,
        Square
    };

    enum class FenceStatus
    {
        Signaled,
        Unsignaled
    };

    template<typename T>
    struct Rect2
    {
        T x;
        T y;
        T width;
        T height;
    };

    using Rect2I = Rect2<int32_t>;
    using Rect2U = Rect2<uint32_t>;
    using Rect2F = Rect2<float>;

    struct ViewportDesc
    {
        float x;
        float y;
        float width;
        float height;
    };

    struct BitmapCreateDesc
    {
        PixelFormat         format;
        uint32_t            width;
        uint32_t            height;
        BitmapUsageFlags    usage;
    };

    struct ImageCreateDesc
    {
        PixelFormat        format;
        uint32_t           width;
        uint32_t           height;
        ImageUsageFlags    usage;
    };

    struct DrawPassCreateDesc
    {
        PixelFormat     format;
        AttachmentOp    loadOp;
        AttachmentOp    storeOp;
    };

    struct GradientFill
    {
        GradientMode        mode;
        GradientWrapping    wrapping;
        InterpolationOp     interpolationOp;
        uint32_t            numColors;
        const ColorRGBA*    colors;
        const float*        positions;
        const float*        midpointPositions;
        bool                reversed;
    };

    struct ColorStateDesc
    {
        ColorRGBA    lineColor;
        ColorMode    fillColorMode;

        union
        {
            ColorRGBA       solidFill;
            GradientFill    gradientFill;
        };
    };

    struct RasterizationStateDesc
    {
        LineJoin    lineJoin;
        LineCap     lineCap;
        float       lineWidth;
        float       miterLimit;
        bool        antiAliasedLine;
        bool        antiAliasedFill;
    };

    struct RenderStateCreateDesc
    {
        const ColorStateDesc*            colorState;
        const RasterizationStateDesc*    rasterizationState;
    };

    struct SubmitDesc
    {
        uint32_t numWaitSemaphores;
        Semaphore* const* waitSemaphores;
        uint32_t numDrawLists;
        DrawList* const* drawLists;
        uint32_t numSignalSemaphores;
        Semaphore* const* signalSemaphores;
    };
}
