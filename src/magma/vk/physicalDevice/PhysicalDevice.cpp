#include "magma/vk/physicalDevice/PhysicalDevice.h"

#include <vector>
#include <stdexcept>
#include <set>
#include <string>
#include <sstream>

#include "magma/vk/vulkan_common.h"

bool QueueFamilyIndices::isComplete() {
    return graphicsFamily.has_value() && computeFamily.has_value() && presentFamily.has_value();
}

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

QueueFamilyIndices PhysicalDevice::findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
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


PhysicalDevice::PhysicalDevice(vk::PhysicalDevice physDevice) :
    _physicalDevice(physDevice),
    _name(physDevice.getProperties().deviceName)
{}

bool PhysicalDevice::checkExtensionSupport(const std::vector<const char*> &extensions) const {
    auto [result, availableExtensions] = _physicalDevice.enumerateDeviceExtensionProperties();
    VK_HPP_CHECK_ERR(result, "failed to enumerate device extension properties");

    std::set<std::string> requiredExtensions(extensions.begin(), extensions.end());
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

SwapChainSupportInfo PhysicalDevice::getSwapChainSupportInfo(vk::SurfaceKHR surface) const {
    SwapChainSupportInfo details;

    details.capabilities = _physicalDevice.getSurfaceCapabilitiesKHR(surface).value;
    details.formats      = _physicalDevice.getSurfaceFormatsKHR(surface).value;
    details.presentModes = _physicalDevice.getSurfacePresentModesKHR(surface).value;

    return details;
}

void PhysicalDevice::initInds(vk::SurfaceKHR surface) {
    _inds = findQueueFamilies(_physicalDevice, surface);
}

