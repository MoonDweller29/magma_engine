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

bool PhysicalDevice::queueFamilyHasSurfaceSupport(uint32_t queueFamilyIndex, vk::SurfaceKHR surface) {
    return _physicalDevice.getSurfaceSupportKHR(queueFamilyIndex, surface).value;
}

static std::string to_string(const vk::QueueFamilyProperties& queueFamily, int i, PhysicalDevice &device, vk::SurfaceKHR surface) {
    std::stringstream ss;
    ss << i << " : " << to_string(queueFamily.queueFlags);
    if (device.queueFamilyHasSurfaceSupport(i, surface)) {
        ss << " - Surface Support";
    }
    ss << " - " << queueFamily.queueCount;

    return ss.str();
}

QueueFamilyIndices PhysicalDevice::findQueueFamilies(vk::SurfaceKHR surface) {
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = _physicalDevice.getQueueFamilyProperties();
    printQueueFamilies(surface);

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
            indices.graphicsFamily = i;
        }
        if (queueFamily.queueFlags & vk::QueueFlagBits::eCompute) {
            indices.computeFamily = i;
        }
        if (queueFamilyHasSurfaceSupport(i, surface)) {
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

uint32_t PhysicalDevice::findMemoryTypeInd(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = _physicalDevice.getMemoryProperties();

    uint32_t memTypeInd = -1;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        bool isSuitableType = typeFilter & (1 << i);
        bool areSuitableProperties = (memProperties.memoryTypes[i].propertyFlags & properties) == properties;
        if (isSuitableType && areSuitableProperties) {
            memTypeInd = i;
            break;
        }
    }

    if (memTypeInd == -1) {
        LOG_AND_THROW std::runtime_error("failed to find suitable memory type!");
    }

    return memTypeInd;
}

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
    _inds = findQueueFamilies(surface);
}

void PhysicalDevice::printQueueFamilies(vk::SurfaceKHR surface) {
    std::vector<vk::QueueFamilyProperties> queueFamilies = _physicalDevice.getQueueFamilyProperties();

    std::stringstream ss;
    ss << _name << " : QUEUE FAMILIES - " << queueFamilies.size() << std::endl;
    int ind = 0;
    for (const auto& queueFamily : queueFamilies) {
        ss << to_string(queueFamily, ind, *this, surface) << std::endl;
        ++ind;
    }

    LOG_INFO(ss.str());
}


