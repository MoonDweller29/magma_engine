#include "magma/vk/physicalDevice/HardwareManager.h"

#include <iostream>
#include <set>
#include <string>

#include "magma/vk/vulkan_common.h"

HardwareManager::HardwareManager(vk::Instance instance) :
    _instance(instance)
{}

static VkBool32 hasSurfaceSupportKHR(
        vk::PhysicalDevice device,
        uint32_t queueFamilyIndex,
        vk::SurfaceKHR surface
) {
    return device.getSurfaceSupportKHR(queueFamilyIndex, surface).value;
}

static std::string to_string(const vk::QueueFamilyProperties& queueFamily, int i, vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    std::stringstream ss;
    ss << i << " : " << to_string(queueFamily.queueFlags);
    if (hasSurfaceSupportKHR(device, i, surface)) {
        ss << " - Surface Support";
    }
    ss << " - " << queueFamily.queueCount;

    return ss.str();
}

QueueFamilyIndices findQueueFamilies(
        vk::PhysicalDevice device, vk::SurfaceKHR surface
) {
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    std::cout << "PHYS DEVICE FAMILIES " << device << std::endl;
    int ind = 0;
    for (const auto& queueFamily : queueFamilies) {
        std::cout << to_string(queueFamily, ind, device, surface) << std::endl;
        ++ind;
    }


    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }
        if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
            indices.computeFamily = i;
        }
        if (hasSurfaceSupportKHR(device, i, surface)) {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
            break;

        i++;
    }

    return indices;
}

bool isSuitable(const PhysicalDevice &device, vk::SurfaceKHR surface) {
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();
    QueueFamilyIndices indices = findQueueFamilies(device.device(), surface);
    bool extensionsSupported = device.checkExtensionSupport(PhysicalDevice::extensions);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportInfo swapChainSupport = device.getSwapChainSupportInfo(surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate && deviceFeatures.samplerAnisotropy;
}

int HardwareManager::rateSuitability(const PhysicalDevice &device, vk::SurfaceKHR surface) {
    if (!isSuitable(device, surface))
        return 0;

    int score = 0;

    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}


std::vector<PhysicalDevice> HardwareManager::acquirePhysicalDevicesList() {
    auto [result, vkDevices] = _instance.enumeratePhysicalDevices();
    if (vkDevices.empty()) {
        VK_ERROR("failed to find GPUs with Vulkan support!");
    }
    std::vector<PhysicalDevice> devices;
    devices.reserve(vkDevices.size());
    for (auto &vkDevice : vkDevices) {
        devices.emplace_back(vkDevice);
    }

    return devices;
}

std::multimap<int, PhysicalDevice> HardwareManager::findSuitableDevices(vk::SurfaceKHR surface) {
    auto devices = acquirePhysicalDevicesList();

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, PhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = rateSuitability(device, surface);
        if (score > 0) {
            candidates.insert(std::make_pair(score, device));
        }
    }

    return candidates;
}

PhysicalDevice HardwareManager::selectBestSuitableDevice(vk::SurfaceKHR surface) {
    std::multimap<int, PhysicalDevice> candidates = findSuitableDevices(surface);
    if (candidates.empty() || candidates.rbegin()->first == 0) {
        VK_ERROR("failed to find a suitable GPU!");
    }
    candidates.rbegin()->second.initInds(surface); //deprecated

    return candidates.rbegin()->second;
}