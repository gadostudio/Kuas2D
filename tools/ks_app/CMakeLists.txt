set(KS_APP_VULKAN_SRC
    "vk/RendererVK.cpp"
    "vk/RendererVK.h")

set(KS_APP_SRC
    "AppMain.cpp"
    "AppMain.h"
    "App.cpp"
    "App.h"
    "Main.cpp"
    "Pch.h"
    "Renderer.h"
    ${KS_APP_VULKAN_SRC})

add_library(ks_app STATIC ${KS_APP_SRC})
target_link_libraries(ks_app PUBLIC SDL2::SDL2-static kuas)
target_include_directories(ks_app PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)