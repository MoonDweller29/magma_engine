#pragma once
#include <vulkan/vulkan.h>
#include <stdexcept>
#include <vector>
#include "magma/app/log.hpp"

#define VK_CHECK_ERR(result, message)      \
{                                          \
    if (result != VK_SUCCESS) {            \
       LOG_ERROR(message);                 \
       throw std::runtime_error(message);  \
    }                                      \
}

VkFormat findSupportedFormat(
        VkPhysicalDevice physicalDevice,
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling, VkFormatFeatureFlags features);

VkFormat findDepthFormat(VkPhysicalDevice physicalDevice);