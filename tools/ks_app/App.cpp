#pragma once

#include "App.h"
#include <chrono>

namespace ks_app
{
    App::App(const char* title, uint32_t w, uint32_t h, uint32_t renderer) :
        AppMain(title, w, h, renderer),
        m_device(m_renderer->getKuasDevice())
    {
        KUAS_ASSERT(m_device != nullptr && "Cannot create kuas device");
        init();
    }

    App::~App()
    {
    }

    void App::swapBuffer()
    {
        FrameData& frameData = m_frameData[m_surface->getCurrentBackBuffer()];
        m_device->enqueueSignal(1, &frameData.renderFinishedSemaphore, nullptr);
        m_surface->swapBuffer(1, &frameData.renderFinishedSemaphore);
    }

    void App::init()
    {
        int x, y;
        SDL_GetWindowSize(m_window, &x, &y);

        kuas::SurfaceCreateDesc surfDesc{};
        surfDesc.type = kuas::SurfaceType::SDL;
        surfDesc.format = kuas::PixelFormat::B8G8R8A8_Unorm;
        surfDesc.numSwapBuffer = 2;
        surfDesc.width = x;
        surfDesc.height = y;
        surfDesc.usage = kuas::ImageUsage::CanvasOutputAttachment;
        surfDesc.data0 = (void*)m_window;

        m_device->createSurface(surfDesc, &m_surface);

        kuas::DrawPassCreateDesc dpDesc{};
        dpDesc.format = surfDesc.format;
        dpDesc.loadOp = kuas::AttachmentOp::Clear;
        dpDesc.storeOp = kuas::AttachmentOp::Store;

        m_device->createDrawPass(dpDesc, &m_drawPass);

        for (uint32_t i = 0; i < m_surface->getNumImages(); i++) {
            FrameData& frameData = m_frameData[i];

            kuas::Image* img = m_surface->getImage(i);
            frameData.surfaceImage.reset(kuas::safeAddRef(img));

            m_device->createCanvas(m_drawPass.get(), frameData.surfaceImage.get(), x, y, &frameData.canvas);
            m_device->createSemaphore(&frameData.renderFinishedSemaphore);
        }
    }
}
