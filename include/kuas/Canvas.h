#pragma once

#include "Types.h"
#include "Image.h"
#include "RefCount.h"

namespace kuas
{
    class Canvas : public RefCount
    {
    public:
        virtual ~Canvas() { }

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;

    protected:
        Canvas() { }
    };
}
