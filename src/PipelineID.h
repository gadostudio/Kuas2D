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
            LineSegment,
            FillRect,
            FillRoundedRect,
            FillCircle,
            FillEllipse,

            Count
        };

        static constexpr size_t count = Count;
    };
}
