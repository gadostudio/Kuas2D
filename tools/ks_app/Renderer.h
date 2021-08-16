#pragma once

#include "Pch.h"
#include <kuas/Device.h>

namespace ks_app
{
    class Renderer
    {
    public:
        virtual ~Renderer() { }

        virtual kuas::Ref<kuas::Device>& getKuasDevice() = 0;
    protected:
        Renderer() { }
    };
}
