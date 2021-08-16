#pragma once

#include <kuas/Types.h>
#include <kuas/SyncObjects.h>
#include <kuas/RefCount.h>

namespace kuas
{
    class Surface : public RefCount
    {
    public:
        virtual ~Surface() { }

        virtual Result resize(uint32_t width, uint32_t height) = 0;
        virtual Result swapBuffer(uint32_t numWaitSemaphores, Semaphore* const* semaphores) = 0;
        virtual uint32_t getCurrentBackBuffer() const = 0;
        virtual uint32_t getNumImages() const = 0;
        virtual Image* getImage(uint32_t n) = 0;

    protected:
        Surface() { }
    };
}
