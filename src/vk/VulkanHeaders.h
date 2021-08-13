#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 0
#include "vk_mem_alloc.h"

#define KUAS_VULKAN_FAILED(x) \
    ((x) < VK_SUCCESS)
