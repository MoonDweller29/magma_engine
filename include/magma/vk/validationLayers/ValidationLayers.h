#pragma once
#include <vulkan/vulkan.hpp>

#include <vector>

class ValidationLayers {
public:
    ValidationLayers() = delete;

    static const bool ENABLED;
    static const std::vector<const char*> validationLayers;

    static bool supported();
};
