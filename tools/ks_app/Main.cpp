#include "App.h"

int main(int argc, const char** argv)
{
    std::unique_ptr<ks_app::App> app(ks_app::App::create(argc, argv));
    app->run();
    return 0;
}
