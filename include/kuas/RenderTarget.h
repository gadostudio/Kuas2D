#pragma once

#include "Types.h"
#include "Image.h"
#include "RefCount.h"

namespace kuas
{
    class RenderTarget : public RefCount
    {
    public:
        virtual ~RenderTarget() { }

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

    protected:
        RenderTarget() { }
    };
}
