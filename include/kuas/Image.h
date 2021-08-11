#pragma once

#include "Types.h"
#include "RefCount.h"

namespace kuas
{
    class Image : public RefCount
    {
    public:
        virtual ~Image() { }

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
        virtual PixelFormat getFormat() const = 0;

    protected:
        Image() { }
    };
}
