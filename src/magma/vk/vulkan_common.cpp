#include "magma/vk/vulkan_common.h"

vk::Format findSupportedFormat(
        vk::PhysicalDevice physicalDevice,
        const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling, vk::FormatFeatureFlags features
) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props = physicalDevice.getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    LOG_AND_THROW std::runtime_error("failed to find supported format!");
}

vk::Format findDepthFormat(vk::PhysicalDevice physicalDevice) {
    return findSupportedFormat(
            physicalDevice,
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}
