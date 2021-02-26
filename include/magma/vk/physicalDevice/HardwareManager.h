#pragma once

#include <vulkan/vulkan.hpp>
#include <vector>
#include <map>

#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "magma/vk/physicalDevice/DeviceRequirements.h"

class HardwareManager {
public:
    HardwareManager(vk::Instance instance);

    std::vector<PhysicalDevice> acquirePhysicalDevicesList();
    std::multimap<int, PhysicalDevice> findSuitableDevices(const DeviceRequirements &requirements);
    PhysicalDevice selectBestSuitableDevice(const DeviceRequirements &requirements);
private:
    vk::Instance _instance;

    bool checkQueueFamilies(const PhysicalDevice &device, const DeviceRequirements &requirements);
    bool isDeviceSuitable(const PhysicalDevice &device, const DeviceRequirements &requirements);
    int rateSuitability(const PhysicalDevice &device, const DeviceRequirements &requirements);
};