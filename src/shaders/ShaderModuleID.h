#pragma once

namespace kuas
{
    struct ShaderModuleID
    {
        enum
        {
            BasicPolyFillNoAA_VS,
            BasicPolyFillNoAA_PS,

            Rect_VS,
            Rect_GS,
            Rect_PS,
            RectAA_GS,
            RectAA_PS,

            FillRect_VS,
            FillRect_GS,
            FillRect_PS,
            FillRectAA_GS,
            FillRectAA_PS,

            Circle_VS,
            Circle_GS,
            Circle_PS,

            Count
        };
    };
    
    static constexpr size_t g_numShaderModules = static_cast<size_t>(ShaderModuleID::Count);
}
