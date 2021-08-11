#pragma once

#include "Types.h"
#include "RefCount.h"

namespace kuas
{
    class Semaphore : public RefCount
    {
    public:
        virtual ~Semaphore() { }

    protected:
        Semaphore() { }
    };

    class Fence : public RefCount
    {
    public:
        virtual ~Fence() { }

        virtual Result wait(uint64_t timeout = UINT64_MAX) = 0;
        virtual void reset() = 0;
        virtual FenceStatus getStatus() = 0;

    protected:
        Fence() { }
    };
}
