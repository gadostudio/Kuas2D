#pragma once

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>

#include "vk_mem_alloc.h"

#define KUAS_VULKAN_FAILED(x) \
    ((x) < VK_SUCCESS)
