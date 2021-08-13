#pragma once

#include "Pch.h"
#include <kuas/Device.h>

namespace ks_app
{
    class Renderer
    {
    public:
        virtual ~Renderer() { }

        virtual void resize(uint32_t w, uint32_t h) = 0;
        virtual void swapBuffer() = 0;

        virtual kuas::Ref<kuas::Device>& getKuasDevice() = 0;

    protected:
        Renderer() { }
    };
}
