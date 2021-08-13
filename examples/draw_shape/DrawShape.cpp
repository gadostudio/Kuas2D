#include "App.h"

class DrawShapeApp : public ks_app::App
{
public:
    DrawShapeApp() :
        ks_app::App("Draw Shape", 640, 480, ks_app::App::RendererVulkan)
    {
    }

    void onRender(double dt) override
    {

    }
};

namespace ks_app
{
    App* App::create(int argc, const char** argv)
    {
        return new DrawShapeApp();
    }
}
