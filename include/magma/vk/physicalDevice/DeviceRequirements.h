/**
 * @file DeviceRequirements.h
 * @brief Struct containing required or recommended options for selecting physical device
 * @version 0.1
 * @date 2021-02-18
 */
#pragma once
#include <vulkan/vulkan.hpp>
#include "magma/app/Option.hpp"

struct DeviceRequirements {
    Option<vk::SurfaceKHR> surface;
    Option<vk::PhysicalDeviceType> deviceType;
    Option<bool> samplerAnisotropy;

    std::vector<const char*> requiredExtensions() const;
    std::vector<const char*> recommendedExtensions() const;
};