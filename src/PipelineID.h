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
            FillRect,
            FillRoundedRect,
            FillCircle,
            FillTriangle,

            Count
        };

        static constexpr size_t count = Count;
    };
}
