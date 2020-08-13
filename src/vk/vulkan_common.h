#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>

inline void vk_check_err(const VkResult& result, const char* message)
{
    if (result != VK_SUCCESS)
       throw std::runtime_error(message);
}