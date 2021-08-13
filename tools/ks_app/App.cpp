#pragma once

#include "App.h"

namespace ks_app
{
    App::App(const char* title, uint32_t w, uint32_t h, uint32_t renderer) :
        AppMain(title, w, h, renderer),
        m_device(getRenderer()->getKuasDevice())
    {
        KUAS_ASSERT(m_device != nullptr && "Cannot create kuas device");
    }

    App::~App()
    {
    }
}
