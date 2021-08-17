#pragma once

namespace kuas
{
    struct PipelineID
    {
        enum
        {
            Unknown,
            Rect,
            RoundedRect,
            Circle,
            FillRect,
            FillRoundedRect,
            FillCircle,
            FillTriangle,

            Count
        };

        static constexpr size_t count = Count;
    };
}
