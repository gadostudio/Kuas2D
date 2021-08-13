#pragma once

#include "VulkanHeaders.h"
#include <kuas/BackendFactoryVK.h>
#include <array>
#include <queue>

#include "ImageVK.h"
#include "../util/StaticQueue.h"
#include "../shaders/ShaderModuleID.h"

namespace kuas
{
    class DeviceVK : public Device
    {
    public:
        DeviceVK(const VulkanDeviceCreateDesc& desc);
        ~DeviceVK();

        Result createBitmap(const BitmapCreateDesc& desc, Bitmap** bitmap) override;
        
        Result createImage(const ImageCreateDesc& desc, Image** image) override;
        
        Result createDrawPass(const DrawPassCreateDesc& desc, DrawPass** drawPass) override;
        
        Result createRenderState(DrawPass* drawPass, const RenderStateCreateDesc& desc, RenderState** renderState) override;
        
        Result createRenderTarget(
            DrawPass* drawPass,
            Image* imageBinding,
            uint32_t width,
            uint32_t height,
            RenderTarget** renderTarget) override;

        Result createPathBuffer() override;
        Result createDrawList(DrawList** drawList) override;
        Result createSemaphore(Semaphore** semaphore) override;
        Result createFence(bool signaled, Fence** fence) override;

        void mapBitmap() override;
        void unmapBitmap() override;

        Result enqueueWait(
            uint32_t numWaitSemaphore,
            Semaphore* const* waitSemaphores) override;

        Result enqueueDrawLists(
            uint32_t numDrawLists,
            DrawList* const* drawLists) override;

        Result enqueueSignal(
            uint32_t numSignalSemaphore,
            Semaphore* const* signalSemaphores,
            Fence* signalFence) override;

        Result checkBitmapPixelFormatSupport(PixelFormat format, BitmapUsageFlags usage) override;
        Result checkImagePixelFormatSupport(PixelFormat format, ImageUsageFlags usage) override;

        void destroyImage(ImageVK* image);

        const VulkanFunctions& getFunc() const { return m_fn; }
        VkDevice getDevice() { return m_device; }
        VmaAllocator getAllocator() { return m_allocator; }
        VkPipelineCache getPipelineCache() { return m_pipelineCache; }
        VkPipelineLayout getCommonPipelineLayout() { return m_commonPipelineLayout; }
        VkShaderModule getShaderModule(uint32_t id) { return m_shaderModules[id]; }

    private:
        VkInstance                          m_instance;
        VkPhysicalDevice                    m_physicalDevice;
        VkDevice                            m_device;
        VkQueue                             m_graphicsQueue;
        uint32_t                            m_graphicsQueueFamily;
        VulkanFunctions                     m_fn;
        VmaAllocator                        m_allocator;
        VkPipelineCache                     m_pipelineCache;
        VkPipelineLayout                    m_commonPipelineLayout;

        StaticQueue<VkSemaphore, 64>        m_waitQueue;
        StaticQueue<VkCommandBuffer, 64>    m_cmdBufferQueue;

        std::array<VkShaderModule, g_numShaderModules> m_shaderModules;

        void initFunctionsAuto(PFN_vkGetInstanceProcAddr instanceLoadFn, PFN_vkGetDeviceProcAddr loadFn);
        void initFunctions(const VulkanFunctions& fn);
        void initVma();
        VkShaderModule createShaderModule(size_t codeSize, const uint32_t* code);
        void initShaderModule();
    };
}
