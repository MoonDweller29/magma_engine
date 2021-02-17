#pragma once
#include <vulkan/vulkan.hpp>
#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete();
};

struct SwapChainSupportInfo {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

SwapChainSupportInfo querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

class PhysicalDevice {
public:
    static const std::vector<const char*> extensions;

    PhysicalDevice(vk::PhysicalDevice physDevice);
    ~PhysicalDevice() = default;

    QueueFamilyIndices getQueueFamilyInds() const { return _inds; }
    VkPhysicalDevice   c_device() const { return (VkPhysicalDevice)_physicalDevice; }
    vk::PhysicalDevice device()   const { return _physicalDevice; }

    vk::PhysicalDeviceProperties getProperties() const { return _physicalDevice.getProperties(); }
    vk::PhysicalDeviceFeatures   getFeatures()   const { return _physicalDevice.getFeatures();   }
    bool checkExtensionSupport(const std::vector<const char*> &extensions) const;
    SwapChainSupportInfo getSwapChainSupportInfo(vk::SurfaceKHR surface) const;
    void initInds(vk::SurfaceKHR surface);
private:
    QueueFamilyIndices _inds;
    vk::PhysicalDevice _physicalDevice;

    static QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
};
