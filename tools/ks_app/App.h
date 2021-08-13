#pragma once

#include "AppMain.h"
#include <kuas/BackendFactoryVK.h>

namespace ks_app
{
    class App : public AppMain
    {
    public:
        App(const char* title, uint32_t w, uint32_t h, uint32_t renderer);
        virtual ~App();

        static App* create(int argc, const char** argv);

    private:
        kuas::Ref<kuas::Device> m_device;
    };
}
