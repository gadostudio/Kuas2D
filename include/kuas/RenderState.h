#pragma once

#include "Types.h"
#include "RefCount.h"
#include "math/ColorRGBA.h"

namespace kuas
{
    class RenderState : public RefCount
    {
    public:
        virtual ~RenderState() { }

    protected:
        RenderState() { }
    };
}
