#include "magma/vk/physicalDevice/HardwareManager.h"

#include <iostream>
#include <set>
#include <string>

#include "magma/vk/vulkan_common.h"

HardwareManager::HardwareManager(vk::Instance instance) :
    _instance(instance)
{}

bool HardwareManager::checkQueueFamilies(const PhysicalDevice &device, const DeviceRequirements &requirements) {
    std::vector<vk::QueueFamilyProperties> queueFamilies = device.device().getQueueFamilyProperties();
    int graphicsFamiliesCount = 0;
    int computeFamiliesCount = 0;
    int presentFamiliesCount = 0;

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            ++graphicsFamiliesCount;
        }
        if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
            ++computeFamiliesCount;
        }
        if (requirements.surface.isRequired()){
            if (device.queueFamilyHasSurfaceSupport(i, requirements.surface.getValue())) {
                ++presentFamiliesCount;
            }
        }

        i++;
    }

    if ((requirements.graphicsSupport.isRequired() && (graphicsFamiliesCount == 0)) ||
        (requirements.computeSupport.isRequired() && (computeFamiliesCount == 0)) ||
        (requirements.surface.isRequired() && (presentFamiliesCount == 0)) ) {
        return false;
    }

    return true;
}


bool HardwareManager::isDeviceSuitable(
        const PhysicalDevice &device, const DeviceRequirements &requirements
) {
    if (!device.checkExtensionSupport(requirements.requiredExtensions())) {
        return false;
    }
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    if (requirements.deviceType.isRequired() &&
            (deviceProperties.deviceType != requirements.deviceType.getValue())) {
        return false;
    }
    if (requirements.features.isRequired()) {
        const vk::PhysicalDeviceFeatures& req_features = requirements.features.getValue();
        if (req_features.samplerAnisotropy && !deviceFeatures.samplerAnisotropy) {
            return false;
        }
    }
    if (!checkQueueFamilies(device, requirements)) {
        return false;
    }

    if (requirements.surface.isRequired()) {
        vk::SurfaceKHR surface = requirements.surface.getValue();
        SwapChainSupportInfo swapChainSupport = device.getSwapChainSupportInfo(surface);
        bool swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();

        return swapChainAdequate;
    }

    return true;
}

int HardwareManager::rateSuitability(
        const PhysicalDevice &device, const DeviceRequirements &requirements
) {
    if (!isDeviceSuitable(device, requirements)) {
        return 0;
    }

    int score = 0;

    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    if (requirements.deviceType.isRecommended() &&
        (deviceProperties.deviceType == requirements.deviceType.getValue())) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}


std::vector<PhysicalDevice> HardwareManager::acquirePhysicalDevicesList() {
    auto [result, vkDevices] = _instance.enumeratePhysicalDevices();
    if (vkDevices.empty()) {
        LOG_AND_THROW std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<PhysicalDevice> devices;
    devices.reserve(vkDevices.size());
    for (auto &vkDevice : vkDevices) {
        devices.emplace_back(vkDevice);
    }

    return devices;
}

std::multimap<int, PhysicalDevice> HardwareManager::findSuitableDevices(const DeviceRequirements &requirements) {
    auto devices = acquirePhysicalDevicesList();

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, PhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = rateSuitability(device, requirements);
        if (score > 0) {
            candidates.insert(std::make_pair(score, device));
        }
    }

    return candidates;
}

PhysicalDevice HardwareManager::selectBestSuitableDevice(const DeviceRequirements &requirements) {
    std::multimap<int, PhysicalDevice> candidates = findSuitableDevices(requirements);
    if (candidates.empty() || candidates.rbegin()->first == 0) {
        LOG_AND_THROW std::runtime_error("failed to find a suitable GPU!");
    }
    PhysicalDevice &selectedDevice = candidates.rbegin()->second;
    selectedDevice.initInds(requirements.surface.getValue()); //deprecated

    LOG_INFO("Selected device: ", selectedDevice.getName());

    return selectedDevice;
}
