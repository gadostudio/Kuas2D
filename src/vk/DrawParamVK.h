#pragma once

#include <kuas/math/Mat3.h>
#include <kuas/math/Vec2.h>

namespace kuas
{
    struct DrawParamVK
    {
        struct alignas(16) {
            float r0, r1, r2;
        } m_transformMat[3];

        Vec2F m_screenSize;
    };
}
