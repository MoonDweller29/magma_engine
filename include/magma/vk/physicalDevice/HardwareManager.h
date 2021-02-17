#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>

#include "magma/vk/physicalDevice/PhysicalDevice.h"

class HardwareManager {
public:
    HardwareManager(vk::Instance instance);

    std::vector<PhysicalDevice> acquirePhysicalDevicesList();
    std::multimap<int, PhysicalDevice> findSuitableDevices(vk::SurfaceKHR surface);
    PhysicalDevice selectBestSuitableDevice(vk::SurfaceKHR surface);
private:
    vk::Instance _instance;

    int rateSuitability(const PhysicalDevice &device, vk::SurfaceKHR surface);
};