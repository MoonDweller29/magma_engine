#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>

inline void vk_check_err(const VkResult& result, const char* message)
{
    if (result != VK_SUCCESS)
       throw std::runtime_error(message);
}

VkFormat findSupportedFormat(
        VkPhysicalDevice physicalDevice,
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);