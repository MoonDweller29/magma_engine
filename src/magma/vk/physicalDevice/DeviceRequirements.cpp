#include "magma/vk/physicalDevice/DeviceRequirements.h"

std::vector<const char*> DeviceRequirements::requiredExtensions() const {
    std::vector<const char*> extensions;

    if (surface.isRequired()) {
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    return extensions;
}

std::vector<const char*> DeviceRequirements::recommendedExtensions() const {
    std::vector<const char*> extensions;

    if (surface.isRecommended()) {
        extensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    return extensions;
}