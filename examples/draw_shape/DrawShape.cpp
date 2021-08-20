#include "App.h"

class DrawShapeApp : public ks_app::App
{
public:
    kuas::Ref<kuas::RenderState> renderState;
    std::vector<kuas::Ref<kuas::DrawList>> drawLists;

    DrawShapeApp() :
        ks_app::App("Draw Shape", 640, 480, ks_app::App::RendererVulkan)
    {
    }

    ~DrawShapeApp()
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

        kuas::ColorRGBA color(0.0f);

        for (uint32_t i = 0; i < numSurfaceImages; i++) {
            getDevice()->createDrawList(&drawLists[i]);

            drawLists[i]->begin();
            drawLists[i]->beginDrawPass(getSurfaceDrawPass().get(), getSurfaceRenderTarget(i).get(), &color);
            drawLists[i]->setRenderState(renderState.get());
            drawLists[i]->setLineThickness(1.0f);
            drawLists[i]->setViewport(viewport);
            drawLists[i]->setScissorRect(scissor);
            drawLists[i]->setTransformation(kuas::scale(1.0f, 1.0f));
            //drawLists[i]->setTransformation(kuas::rotate(kuas::degreeToRadian(25.0f)) * kuas::scale(1.0f, 2.5f));

            // Rectangle
            drawLists[i]->setLineColor(kuas::ColorRGBA(1.0f, 0.0f, 0.0f));
            drawLists[i]->drawRect({ 20.0f, 20.0f, 80.0f, 80.0f });
            drawLists[i]->setLineColor(kuas::ColorRGBA(0.0f, 1.0f, 0.0f));
            drawLists[i]->drawRect({ 20.0f, 104.0f, 80.0f, 80.0f });
            drawLists[i]->setLineColor(kuas::ColorRGBA(0.0f, 0.0f, 1.0f));
            drawLists[i]->drawRect({ 20.0f, 188.0f, 80.0f, 80.0f });

            // Rounded Rectangle
            drawLists[i]->setLineColor(kuas::ColorRGBA(1.0f, 0.0f, 0.0f));
            drawLists[i]->drawRoundedRect({ 104.0f, 20.0f, 80.0f, 80.0f }, 15.f);
            drawLists[i]->setLineColor(kuas::ColorRGBA(0.0f, 1.0f, 0.0f));
            drawLists[i]->drawRoundedRect({ 104.0f, 104.0f, 80.0f, 80.0f }, 15.f);
            drawLists[i]->setLineColor(kuas::ColorRGBA(0.0f, 0.0f, 1.0f));
            drawLists[i]->drawRoundedRect({ 104.0f, 188.0f, 80.0f, 80.0f }, 15.f);

            // Fill Rectangle
            drawLists[i]->setFillColor(kuas::ColorRGBA(1.0f, 0.0f, 0.0f));
            drawLists[i]->fillRect({ 200.0f, 20.0f, 80.0f, 80.0f });
            drawLists[i]->setFillColor(kuas::ColorRGBA(0.0f, 1.0f, 0.0f));
            drawLists[i]->fillRect({ 200.0f, 104.0f, 80.0f, 80.0f });
            drawLists[i]->setFillColor(kuas::ColorRGBA(0.0f, 0.0f, 1.0f));
            drawLists[i]->fillRect({ 200.0f, 188.0f, 80.0f, 80.0f });

            // Fill Rounded Rectangle
            drawLists[i]->setFillColor(kuas::ColorRGBA(1.0f, 0.0f, 0.0f));
            drawLists[i]->fillRoundedRect({ 284.0f, 20.0f, 80.f, 80.f }, 15.f);
            drawLists[i]->setFillColor(kuas::ColorRGBA(0.0f, 1.0f, 0.0f));
            drawLists[i]->fillRoundedRect({ 284.0f, 104.0f, 80.f, 80.f }, 15.f);
            drawLists[i]->setFillColor(kuas::ColorRGBA(0.0f, 0.0f, 1.0f));
            drawLists[i]->fillRoundedRect({ 284.0f, 188.0f, 80.f, 80.f }, 15.f);

            // Circle
            drawLists[i]->setLineColor(kuas::ColorRGBA(1.0f, 0.0f, 0.0f));
            drawLists[i]->drawCircle(kuas::Vec2F(420.0f, 60.0f), 40.0f);
            drawLists[i]->setLineColor(kuas::ColorRGBA(0.0f, 1.0f, 0.0f));
            drawLists[i]->drawCircle(kuas::Vec2F(420.0f, 144.0f), 40.0f);
            drawLists[i]->setLineColor(kuas::ColorRGBA(0.0f, 0.0f, 1.0f));
            drawLists[i]->drawCircle(kuas::Vec2F(420.0f, 228.0f), 40.0f);

            // Fill Circle
            drawLists[i]->setFillColor(kuas::ColorRGBA(1.0f, 0.0f, 0.0f));
            drawLists[i]->fillCircle(kuas::Vec2F(504.0f, 60.0f), 40.0f);
            drawLists[i]->setFillColor(kuas::ColorRGBA(0.0f, 1.0f, 0.0f));
            drawLists[i]->fillCircle(kuas::Vec2F(504.0f, 144.0f), 40.0f);
            drawLists[i]->setFillColor(kuas::ColorRGBA(0.0f, 0.0f, 1.0f));
            drawLists[i]->fillCircle(kuas::Vec2F(504.0f, 228.0f), 40.0f);

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
        return new DrawShapeApp();
    }
}
