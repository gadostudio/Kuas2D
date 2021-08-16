#pragma once

#include "AppMain.h"
#include <kuas/BackendFactoryVK.h>
#include <array>

namespace ks_app
{
    class App : public AppMain
    {
    public:
        App(const char* title, uint32_t w, uint32_t h, uint32_t renderer);
        virtual ~App();

        void swapBuffer();

        kuas::Ref<kuas::Device>& getDevice() { return m_device; }
        uint32_t getNumSurfaceImages() { return m_surface->getNumImages(); }
        uint32_t getCurrentBackBuffer() { return m_surface->getCurrentBackBuffer(); }
        kuas::Ref<kuas::DrawPass>& getSurfaceDrawPass() { return m_drawPass; }
        kuas::Ref<kuas::Image>& getSurfaceImage(uint32_t n) { return m_frameData[n].renderTargetImage; }
        kuas::Ref<kuas::RenderTarget>& getSurfaceRenderTarget(uint32_t n) { return m_frameData[n].renderTarget; }

        static App* create(int argc, const char** argv);

    private:
        kuas::Ref<kuas::Device> m_device;
        kuas::Ref<kuas::Surface> m_surface;

        struct FrameData
        {
            kuas::Ref<kuas::Image> renderTargetImage;
            kuas::Ref<kuas::RenderTarget> renderTarget;
            kuas::Ref<kuas::Semaphore> renderFinishedSemaphore;
        };

        std::array<FrameData, 2> m_frameData;
        uint32_t m_currentFrame;

        kuas::Ref<kuas::DrawPass> m_drawPass;

        void init();
    };
}
