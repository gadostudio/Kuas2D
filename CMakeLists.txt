cmake_minimum_required(VERSION 3.14)

project(kuas)

add_subdirectory(extern)

set(KUAS_VULKAN_BACKEND_SRC
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
    "src/vk/RenderTargetVK.cpp"
    "src/vk/RenderTargetVK.h"
    "src/vk/SemaphoreVK.cpp"
    "src/vk/SemaphoreVK.h"
    "src/vk/VertexAttributeTypesVK.cpp"
    "src/vk/VertexAttributeTypesVK.h"
    "src/vk/VmaStub.cpp"
    "src/vk/vk_mem_alloc.h"
    "src/vk/VulkanConvert.h"
    "src/vk/VulkanHeaders.h"
    "src/BackendFactoryVK.cpp")

set(KUAS_SHADER_SRC
    "src/shaders/BasicPolyFillNoAA_FS.h"
    "src/shaders/BasicPolyFillNoAA_VS.h")

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
target_link_libraries(kuas PUBLIC Vulkan-Headers spdlog::spdlog)
target_include_directories(
    kuas
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
           $<INSTALL_INTERFACE:include>)

add_subdirectory(tools/ks_app)
add_subdirectory(examples)