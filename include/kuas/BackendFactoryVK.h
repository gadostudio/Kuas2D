#pragma once

#include "Device.h"
#include <vulkan/vulkan.h>

namespace kuas
{
    struct VulkanFunctions
    {
        // instance functions
        PFN_vkGetPhysicalDeviceProperties               vkGetPhysicalDeviceProperties;
        PFN_vkGetPhysicalDeviceMemoryProperties         vkGetPhysicalDeviceMemoryProperties;
        PFN_vkGetPhysicalDeviceImageFormatProperties    vkGetPhysicalDeviceImageFormatProperties;

        // device functions
        PFN_vkAllocateCommandBuffers            vkAllocateCommandBuffers;
        PFN_vkAllocateDescriptorSets            vkAllocateDescriptorSets;
        PFN_vkAllocateMemory                    vkAllocateMemory;
        PFN_vkBeginCommandBuffer                vkBeginCommandBuffer;
        PFN_vkBindBufferMemory                  vkBindBufferMemory;
        PFN_vkBindImageMemory                   vkBindImageMemory;
        PFN_vkCmdBeginQuery                     vkCmdBeginQuery;
        PFN_vkCmdBeginRenderPass                vkCmdBeginRenderPass;
        PFN_vkCmdBindDescriptorSets             vkCmdBindDescriptorSets;
        PFN_vkCmdBindIndexBuffer                vkCmdBindIndexBuffer;
        PFN_vkCmdBindPipeline                   vkCmdBindPipeline;
        PFN_vkCmdBindVertexBuffers              vkCmdBindVertexBuffers;
        PFN_vkCmdBlitImage                      vkCmdBlitImage;
        PFN_vkCmdClearAttachments               vkCmdClearAttachments;
        PFN_vkCmdClearColorImage                vkCmdClearColorImage;
        PFN_vkCmdClearDepthStencilImage         vkCmdClearDepthStencilImage;
        PFN_vkCmdCopyBuffer                     vkCmdCopyBuffer;
        PFN_vkCmdCopyBufferToImage              vkCmdCopyBufferToImage;
        PFN_vkCmdCopyImage                      vkCmdCopyImage;
        PFN_vkCmdCopyImageToBuffer              vkCmdCopyImageToBuffer;
        PFN_vkCmdCopyQueryPoolResults           vkCmdCopyQueryPoolResults;
        PFN_vkCmdDispatch                       vkCmdDispatch;
        PFN_vkCmdDispatchIndirect               vkCmdDispatchIndirect;
        PFN_vkCmdDraw                           vkCmdDraw;
        PFN_vkCmdDrawIndexed                    vkCmdDrawIndexed;
        PFN_vkCmdDrawIndexedIndirect            vkCmdDrawIndexedIndirect;
        PFN_vkCmdDrawIndirect                   vkCmdDrawIndirect;
        PFN_vkCmdEndQuery                       vkCmdEndQuery;
        PFN_vkCmdEndRenderPass                  vkCmdEndRenderPass;
        PFN_vkCmdExecuteCommands                vkCmdExecuteCommands;
        PFN_vkCmdFillBuffer                     vkCmdFillBuffer;
        PFN_vkCmdNextSubpass                    vkCmdNextSubpass;
        PFN_vkCmdPipelineBarrier                vkCmdPipelineBarrier;
        PFN_vkCmdPushConstants                  vkCmdPushConstants;
        PFN_vkCmdResetEvent                     vkCmdResetEvent;
        PFN_vkCmdResetQueryPool                 vkCmdResetQueryPool;
        PFN_vkCmdResolveImage                   vkCmdResolveImage;
        PFN_vkCmdSetBlendConstants              vkCmdSetBlendConstants;
        PFN_vkCmdSetDepthBias                   vkCmdSetDepthBias;
        PFN_vkCmdSetDepthBounds                 vkCmdSetDepthBounds;
        PFN_vkCmdSetEvent                       vkCmdSetEvent;
        PFN_vkCmdSetLineWidth                   vkCmdSetLineWidth;
        PFN_vkCmdSetScissor                     vkCmdSetScissor;
        PFN_vkCmdSetStencilCompareMask          vkCmdSetStencilCompareMask;
        PFN_vkCmdSetStencilReference            vkCmdSetStencilReference;
        PFN_vkCmdSetStencilWriteMask            vkCmdSetStencilWriteMask;
        PFN_vkCmdSetViewport                    vkCmdSetViewport;
        PFN_vkCmdUpdateBuffer                   vkCmdUpdateBuffer;
        PFN_vkCmdWaitEvents                     vkCmdWaitEvents;
        PFN_vkCmdWriteTimestamp                 vkCmdWriteTimestamp;
        PFN_vkCreateBuffer                      vkCreateBuffer;
        PFN_vkCreateBufferView                  vkCreateBufferView;
        PFN_vkCreateCommandPool                 vkCreateCommandPool;
        PFN_vkCreateComputePipelines            vkCreateComputePipelines;
        PFN_vkCreateDescriptorPool              vkCreateDescriptorPool;
        PFN_vkCreateDescriptorSetLayout         vkCreateDescriptorSetLayout;
        PFN_vkCreateEvent                       vkCreateEvent;
        PFN_vkCreateFence                       vkCreateFence;
        PFN_vkCreateFramebuffer                 vkCreateFramebuffer;
        PFN_vkCreateGraphicsPipelines           vkCreateGraphicsPipelines;
        PFN_vkCreateImage                       vkCreateImage;
        PFN_vkCreateImageView                   vkCreateImageView;
        PFN_vkCreatePipelineCache               vkCreatePipelineCache;
        PFN_vkCreatePipelineLayout              vkCreatePipelineLayout;
        PFN_vkCreateQueryPool                   vkCreateQueryPool;
        PFN_vkCreateRenderPass                  vkCreateRenderPass;
        PFN_vkCreateSampler                     vkCreateSampler;
        PFN_vkCreateSemaphore                   vkCreateSemaphore;
        PFN_vkCreateShaderModule                vkCreateShaderModule;
        PFN_vkDestroyBuffer                     vkDestroyBuffer;
        PFN_vkDestroyBufferView                 vkDestroyBufferView;
        PFN_vkDestroyCommandPool                vkDestroyCommandPool;
        PFN_vkDestroyDescriptorPool             vkDestroyDescriptorPool;
        PFN_vkDestroyDescriptorSetLayout        vkDestroyDescriptorSetLayout;
        PFN_vkDestroyDevice                     vkDestroyDevice;
        PFN_vkDestroyEvent                      vkDestroyEvent;
        PFN_vkDestroyFence                      vkDestroyFence;
        PFN_vkDestroyFramebuffer                vkDestroyFramebuffer;
        PFN_vkDestroyImage                      vkDestroyImage;
        PFN_vkDestroyImageView                  vkDestroyImageView;
        PFN_vkDestroyPipeline                   vkDestroyPipeline;
        PFN_vkDestroyPipelineCache              vkDestroyPipelineCache;
        PFN_vkDestroyPipelineLayout             vkDestroyPipelineLayout;
        PFN_vkDestroyQueryPool                  vkDestroyQueryPool;
        PFN_vkDestroyRenderPass                 vkDestroyRenderPass;
        PFN_vkDestroySampler                    vkDestroySampler;
        PFN_vkDestroySemaphore                  vkDestroySemaphore;
        PFN_vkDestroyShaderModule               vkDestroyShaderModule;
        PFN_vkDeviceWaitIdle                    vkDeviceWaitIdle;
        PFN_vkEndCommandBuffer                  vkEndCommandBuffer;
        PFN_vkFlushMappedMemoryRanges           vkFlushMappedMemoryRanges;
        PFN_vkFreeCommandBuffers                vkFreeCommandBuffers;
        PFN_vkFreeDescriptorSets                vkFreeDescriptorSets;
        PFN_vkFreeMemory                        vkFreeMemory;
        PFN_vkGetBufferMemoryRequirements       vkGetBufferMemoryRequirements;
        PFN_vkGetDeviceMemoryCommitment         vkGetDeviceMemoryCommitment;
        PFN_vkGetDeviceQueue                    vkGetDeviceQueue;
        PFN_vkGetEventStatus                    vkGetEventStatus;
        PFN_vkGetFenceStatus                    vkGetFenceStatus;
        PFN_vkGetImageMemoryRequirements        vkGetImageMemoryRequirements;
        PFN_vkGetImageSparseMemoryRequirements  vkGetImageSparseMemoryRequirements;
        PFN_vkGetImageSubresourceLayout         vkGetImageSubresourceLayout;
        PFN_vkGetPipelineCacheData              vkGetPipelineCacheData;
        PFN_vkGetQueryPoolResults               vkGetQueryPoolResults;
        PFN_vkGetRenderAreaGranularity          vkGetRenderAreaGranularity;
        PFN_vkInvalidateMappedMemoryRanges      vkInvalidateMappedMemoryRanges;
        PFN_vkMapMemory                         vkMapMemory;
        PFN_vkMergePipelineCaches               vkMergePipelineCaches;
        PFN_vkQueueBindSparse                   vkQueueBindSparse;
        PFN_vkQueueSubmit                       vkQueueSubmit;
        PFN_vkQueueWaitIdle                     vkQueueWaitIdle;
        PFN_vkResetCommandBuffer                vkResetCommandBuffer;
        PFN_vkResetCommandPool                  vkResetCommandPool;
        PFN_vkResetDescriptorPool               vkResetDescriptorPool;
        PFN_vkResetEvent                        vkResetEvent;
        PFN_vkResetFences                       vkResetFences;
        PFN_vkSetEvent                          vkSetEvent;
        PFN_vkUnmapMemory                       vkUnmapMemory;
        PFN_vkUpdateDescriptorSets              vkUpdateDescriptorSets;
        PFN_vkWaitForFences                     vkWaitForFences;
    };

    struct VulkanDeviceCreateDesc
    {
        VkInstance                      instance;
        VkPhysicalDevice                physicalDevice;
        VkDevice                        device;
        uint32_t                        graphicsQueueFamilyIndex;
        VkQueue                         graphicsQueue;
        PFN_vkGetInstanceProcAddr       instanceProcAddrFn;
        PFN_vkGetDeviceProcAddr         deviceProcAddrFn;
        const VulkanFunctions*          functions;
    };

    extern Result createBackendDeviceVk(const VulkanDeviceCreateDesc& desc, Device** device);
    extern Result getBackendDeviceFnPtrsVk(Device* device, const VulkanFunctions& fnPtrs);

    extern Result createBackendImageVk(Device* device, const ImageCreateDesc& desc, VkImage vkImage, VkImageView view, Image** image);
    extern void getBackendImageVk(
        Image* image,
        VkImageUsageFlags* outUsageFlags,
        VkImageView* outImageView,
        VkImage* outBackendImage);

    extern Result createBackendSemaphoreVk(Device* device, VkSemaphore backendSemaphore, bool managed);
    extern VkSemaphore getBackendSemaphoreVk(Semaphore* semaphore);
}
