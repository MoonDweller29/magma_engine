#pragma once
#include <vulkan/vulkan.h>

struct Buffer
{
    VkBuffer buf = VK_NULL_HANDLE;
    VkDeviceMemory mem = VK_NULL_HANDLE;
};