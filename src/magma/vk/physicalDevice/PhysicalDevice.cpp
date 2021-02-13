#include "magma/vk/physicalDevice/PhysicalDevice.h"

#include <vector>
#include <stdexcept>
#include <map>
#include <set>
#include <string>
#include <sstream>

#include "magma/vk/vulkan_common.h"

bool QueueFamilyIndices::isComplete() {
    return graphicsFamily.has_value() && computeFamily.has_value() && presentFamily.has_value();
}

const std::vector<const char*> PhysicalDevice::extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

static VkBool32 hasSurfaceSupportKHR(
        vk::PhysicalDevice device,
        uint32_t queueFamilyIndex,
        vk::SurfaceKHR surface
) {
    return device.getSurfaceSupportKHR(queueFamilyIndex, surface).value;
}

std::string to_string(const vk::QueueFamilyProperties& queueFamily, int i, vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    std::stringstream ss;
    ss << i << " : " << to_string(queueFamily.queueFlags);
    if (hasSurfaceSupportKHR(device, i, surface)) {
        ss << " - Surface Support";
    }
    ss << " - " << queueFamily.queueCount;

    return ss.str();
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies(
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

bool PhysicalDevice::checkDeviceExtensionSupport(vk::PhysicalDevice device) {
    std::vector<vk::ExtensionProperties> availableExtensions = device.enumerateDeviceExtensionProperties().value;

    std::set<std::string> requiredExtensions(PhysicalDevice::extensions.begin(), PhysicalDevice::extensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool PhysicalDevice::isSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();
    QueueFamilyIndices indices = findQueueFamilies(device, surface);
    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate && deviceFeatures.samplerAnisotropy;
}

int PhysicalDevice::rateSuitability(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    vk::PhysicalDeviceProperties deviceProperties = device.getProperties();
    vk::PhysicalDeviceFeatures deviceFeatures = device.getFeatures();

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
        score += 1000;
    }

    // Maximum possible size of textures affects graphics quality
    score += deviceProperties.limits.maxImageDimension2D;

    if (!isSuitable(device, surface))
        return 0;

    return score;
}

std::vector<vk::PhysicalDevice> acquirePhysicalDevicesList(vk::Instance instance) {
    auto [result, devices] = instance.enumeratePhysicalDevices();
    if (devices.empty()) {
        VK_ERROR("failed to find GPUs with Vulkan support!");
    }

    return devices;
}

vk::PhysicalDevice PhysicalDevice::selectDevice(vk::Instance instance, vk::SurfaceKHR surface) {
    vk::PhysicalDevice physicalDevice;

    auto devices = acquirePhysicalDevicesList(instance);

    // Use an ordered map to automatically sort candidates by increasing score
    std::multimap<int, vk::PhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = rateSuitability(device, surface);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        physicalDevice = candidates.rbegin()->second;
    } else {
        VK_ERROR("failed to find a suitable GPU!");
    }

    return physicalDevice;
}

PhysicalDevice::PhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface) {
    _physicalDevice = selectDevice(instance, surface);
    _inds = findQueueFamilies(_physicalDevice, surface);
}

SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
    SwapChainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(surface).value;
    details.formats = device.getSurfaceFormatsKHR(surface).value;
    details.presentModes = device.getSurfacePresentModesKHR(surface).value;

    return details;
}

