#include "magma/vk/validationLayers/ValidationLayers.h"

#include "magma/vk/vulkan_common.h"
#include "magma/app/log.hpp"
#include <cstring>

#ifdef NDEBUG
    const bool ValidationLayers::ENABLED = false;
#else
    const bool ValidationLayers::ENABLED = true;
#endif

const std::vector<const char*> ValidationLayers::validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

bool ValidationLayers::supported() {
    auto [result, availableLayers] = vk::enumerateInstanceLayerProperties();

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            LOG_WARNING("Validation layer \"", layerName, "\" not found");
            return false;
        }
    }

    return true;
}
