#pragma once

#include "Types.h"
#include "RefCount.h"

namespace kuas
{
    class DrawPass : public RefCount
    {
    public:
        virtual ~DrawPass() { }

    protected:
        DrawPass() { }
    };
}
