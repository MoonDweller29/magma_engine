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

    vk::PhysicalDevice device()   const { return _physicalDevice; }

    uint32_t findMemoryTypeInd(uint32_t typeFilter, vk::MemoryPropertyFlags properties);

    QueueFamilyIndices getQueueFamilyInds() const { return _inds; }
    void initInds(vk::SurfaceKHR surface);
    void printQueueFamilies(vk::SurfaceKHR surface);
    bool queueFamilyHasSurfaceSupport(uint32_t queueFamilyIndex, vk::SurfaceKHR surface);


    const std::string           &getName()       const { return _name; }
    vk::PhysicalDeviceProperties getProperties() const { return _physicalDevice.getProperties(); }
    vk::PhysicalDeviceFeatures   getFeatures()   const { return _physicalDevice.getFeatures();   }
    bool checkExtensionSupport(const std::vector<const char*> &extensions) const;
    SwapChainSupportInfo getSwapChainSupportInfo(vk::SurfaceKHR surface) const;
private:
    QueueFamilyIndices _inds;
    vk::PhysicalDevice _physicalDevice;
    std::string _name;

    QueueFamilyIndices findQueueFamilies(vk::SurfaceKHR surface);
};
