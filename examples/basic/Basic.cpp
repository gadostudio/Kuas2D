#include "App.h"

class BasicApp : public ks_app::App
{
public:
    kuas::Ref<kuas::RenderState> renderState;
    std::vector<kuas::Ref<kuas::DrawList>> drawLists;

    BasicApp() :
        ks_app::App("Basic", 640, 480, ks_app::App::RendererVulkan)
    {
    }

    ~BasicApp()
    {
        getDevice()->waitIdle();
    }

    void onInit() override
    {
        kuas::ColorStateDesc colorState{};
        colorState.fillColorMode = kuas::ColorMode::Solid;

        kuas::RasterizationStateDesc rasterState{};
        rasterState.lineJoin = kuas::LineJoin::Miter;
        rasterState.lineCap = kuas::LineCap::None;
        rasterState.lineWidth = 0.0f;
        rasterState.miterLimit = 0.1f;
        rasterState.antiAliasedLine = true;
        rasterState.antiAliasedFill = true;

        kuas::RenderStateCreateDesc rsDesc{};
        rsDesc.colorState = &colorState;
        rsDesc.rasterizationState = &rasterState;

        getDevice()->createRenderState(getSurfaceDrawPass().get(), rsDesc, &renderState);

        uint32_t numSurfaceImages = getNumSurfaceImages();
        drawLists.resize(numSurfaceImages);

        kuas::ViewportDesc viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)640.0f;
        viewport.height = (float)480.0f;

        kuas::Rect2I scissor{};
        scissor.x = 0;
        scissor.y = 0;
        scissor.width = 640;
        scissor.height = 480;

        kuas::ColorRGBA color(1.0f);

        for (uint32_t i = 0; i < numSurfaceImages; i++) {
            getDevice()->createDrawList(&drawLists[i]);

            drawLists[i]->begin();
            drawLists[i]->beginDrawPass(getSurfaceDrawPass().get(), getSurfaceRenderTarget(i).get(), &color);
            drawLists[i]->setRenderState(renderState.get());
            drawLists[i]->setViewport(viewport);
            drawLists[i]->setScissorRect(scissor);
            drawLists[i]->setLineWidth(1.0f);
            drawLists[i]->drawRect({ 20.0f, 20.0f, 50.0f, 50.0f });
            drawLists[i]->drawRoundedRect({ 20.0f, 75.0f, 50.0f, 50.0f }, 10.0f);
            drawLists[i]->drawCircle(kuas::Vec2F(45.0f, 155.0f), 25.0f);
            drawLists[i]->fillRect({ 75.0f, 20.0f, 50.0f, 50.0f });
            drawLists[i]->fillRoundedRect({ 75.0f, 75.0f, 50.0f, 50.0f }, 10.0f);
            drawLists[i]->fillCircle(kuas::Vec2F(100.0f, 155.0f), 25.0f);
            drawLists[i]->fillEllipse(kuas::Vec2F(155.0f, 100.0f), 25.0f, 80.0f);
            
            for (int j = 0; j < 10; j++) {
                float f = static_cast<float>(j) * 10.0f;
                float w = static_cast<float>(j + 1) * 0.5f;
                drawLists[i]->setLineWidth(w);
                drawLists[i]->drawLine(kuas::Vec2F(200.0f + f, 180.0f), kuas::Vec2F(220.0f + f, 20.0f));
            }

            drawLists[i]->endDrawPass();
            drawLists[i]->end();
        }
    }

    void onRender(double dt) override
    {
        getDevice()->enqueueDrawLists(1, &drawLists[getCurrentBackBuffer()]);
        swapBuffer();
    }
};

namespace ks_app
{
    App* App::create(int argc, const char** argv)
    {
        return new BasicApp();
    }
}
