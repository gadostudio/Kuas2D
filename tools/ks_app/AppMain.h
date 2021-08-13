#pragma once

#include <cstdint>
#include <memory>
#include <SDL.h>
#undef main

#include "Renderer.h"

namespace ks_app
{
    class AppMain
    {
    public:
        enum
        {
            RendererVulkan
        };

        AppMain(const char* title, uint32_t w, uint32_t h, uint32_t renderer);
        virtual ~AppMain();

        void run();

        Renderer* getRenderer() { return m_renderer.get(); }

        virtual void onRender(double dt) = 0;

    private:
        std::unique_ptr<Renderer> m_renderer;
        SDL_Window* m_window;
    };
}
