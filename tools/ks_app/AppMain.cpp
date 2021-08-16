#include "AppMain.h"
#include "vk/RendererVK.h"

#include <stdexcept>

namespace ks_app
{
    AppMain::AppMain(const char* title, uint32_t w, uint32_t h, uint32_t renderer) :
        m_window(nullptr)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
            throw std::runtime_error("Cannot create app");
        }

        m_window = SDL_CreateWindow(
            title, SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED, w, h,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_VULKAN);

        if (m_window == nullptr) {
            throw std::runtime_error("Cannot create window");
        }

        m_renderer.reset(new RendererVK(m_window, w, h));
    }

    AppMain::~AppMain()
    {
        SDL_DestroyWindow(m_window);
        SDL_Quit();
    }

    void AppMain::run()
    {
        bool running = true;

        onInit();

        while (running) {
            SDL_Event event{};

            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_WINDOWEVENT:
                        switch (event.window.event) {
                            case SDL_WINDOWEVENT_RESIZED:
                                onResize(event.window.data1, event.window.data2);
                                break;
                        }
                        break;
                    case SDL_QUIT:
                        running = false;
                        break;
                }
            }

            onRender(0.0);
        }
    }

    void AppMain::onResize(uint32_t w, uint32_t h)
    {
    }
}