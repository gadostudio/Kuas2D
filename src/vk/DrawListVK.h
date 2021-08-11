#pragma once

#include <kuas/BackendFactoryVK.h>
#include "VulkanHeaders.h"
#include "../CommandDispatcher.h"

#define NUM_INTERNAL_CMD_BUFFERS 4

namespace kuas
{
    class DeviceVK;

    class DrawListVK : public CommandDispatcher
    {
    public:
        DrawListVK(VkCommandPool cmdPool, DeviceVK* parentDevice);
        virtual ~DrawListVK();

        void begin() override;
        void end() override;

        friend class DeviceVK;

    private:
        VkCommandPool           m_cmdPool;
        VkCommandBuffer         m_cmdBuffer;
        VkBuffer                m_idxBuffer;
        VmaAllocation           m_idxBufferAlloc;
        uint32_t                m_idxBufferSize;
        VkBuffer                m_vtxBuffer;
        VmaAllocation           m_vtxBufferAlloc;
        uint32_t                m_vtxBufferSize;
        VmaAllocator            m_allocator;
        const VulkanFunctions&  m_fn;
        DeviceVK*               m_parentDevice;
        
        void init();
        void destroy();
        void reserveBuffer(uint32_t i);
    };
}
