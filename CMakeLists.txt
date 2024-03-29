cmake_minimum_required(VERSION 3.14)

project(kuas)

option(KUAS_BUILD_TESTS "Build test" OFF)

add_subdirectory(extern)

set(KUAS_VULKAN_BACKEND_SRC
    "src/vk/CanvasVK.cpp"
    "src/vk/CanvasVK.h"
    "src/vk/DeviceVK.cpp"
    "src/vk/DeviceVK.h"
    "src/vk/DrawListVK.cpp"
    "src/vk/DrawListVK.h"
    "src/vk/DrawPassVK.cpp"
    "src/vk/DrawPassVK.h"
    "src/vk/FenceVK.cpp"
    "src/vk/FenceVK.h"
    "src/vk/ImageVK.cpp"
    "src/vk/ImageVK.h"
    "src/vk/PipelineBuilderVK.cpp"
    "src/vk/PipelineBuilderVK.h"
    "src/vk/RenderStateVK.h"
    "src/vk/RenderStateVK.cpp"
    "src/vk/SemaphoreVK.cpp"
    "src/vk/SemaphoreVK.h"
    "src/vk/SurfaceVK.cpp"
    "src/vk/SurfaceVK.h"
    "src/vk/VertexAttributeTypesVK.cpp"
    "src/vk/VertexAttributeTypesVK.h"
    "src/vk/VmaStub.cpp"
    "src/vk/vk_mem_alloc.h"
    "src/vk/VulkanConvert.h"
    "src/vk/VulkanHeaders.h"
    "src/BackendFactoryVK.cpp")

set(KUAS_SRC
    "src/DrawList.cpp"
    "src/CommandDispatcher.cpp"
    "src/CommandDispatcher.h"
    "src/CommandList.cpp"
    "src/CommandList.h"
    "src/ShaderBuilder.cpp"
    "src/ShaderBuilder.h"
    "src/shaders/ShaderModuleID.h"
    ${KUAS_VULKAN_BACKEND_SRC})

add_library(kuas STATIC ${KUAS_SRC})
target_link_libraries(kuas PUBLIC Vulkan-Headers spdlog::spdlog SDL2::SDL2-static)
target_include_directories(
    kuas
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)

add_subdirectory(tools/ks_app)
add_subdirectory(examples)

if(${KUAS_BUILD_TEST})
    enable_testing()
    add_subdirectory(test)
endif()
