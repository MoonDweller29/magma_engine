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

class PhysicalDevice {
public:
    PhysicalDevice(vk::PhysicalDevice physDevice);
    ~PhysicalDevice() = default;

    VkPhysicalDevice   c_device() const { return (VkPhysicalDevice)_physicalDevice; }
    vk::PhysicalDevice device()   const { return _physicalDevice; }

    QueueFamilyIndices getQueueFamilyInds() const { return _inds; }
    void initInds(vk::SurfaceKHR surface);

    const std::string &getName() const { return _name; }
    vk::PhysicalDeviceProperties getProperties() const { return _physicalDevice.getProperties(); }
    vk::PhysicalDeviceFeatures   getFeatures()   const { return _physicalDevice.getFeatures();   }
    bool checkExtensionSupport(const std::vector<const char*> &extensions) const;
    SwapChainSupportInfo getSwapChainSupportInfo(vk::SurfaceKHR surface) const;
private:
    QueueFamilyIndices _inds;
    vk::PhysicalDevice _physicalDevice;
    std::string _name;

    static QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
};
