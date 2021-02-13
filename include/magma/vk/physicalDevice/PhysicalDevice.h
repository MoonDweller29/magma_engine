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

struct SwapChainSupportDetails {
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

SwapChainSupportDetails querySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface);

class PhysicalDevice {
public:
    static const std::vector<const char*> extensions;

    PhysicalDevice(vk::Instance instance, vk::SurfaceKHR surface);
    ~PhysicalDevice() = default;

    QueueFamilyIndices getQueueFamilyInds() const { return _inds; }
    VkPhysicalDevice   c_device() const { return (VkPhysicalDevice)_physicalDevice; }
    vk::PhysicalDevice device()   const { return _physicalDevice; }
private:
    QueueFamilyIndices _inds;
    vk::PhysicalDevice _physicalDevice;

    static bool checkDeviceExtensionSupport(vk::PhysicalDevice device);
    static bool isSuitable(vk::PhysicalDevice device, vk::SurfaceKHR surface);
    static int rateSuitability(vk::PhysicalDevice device, vk::SurfaceKHR surface);
    static vk::PhysicalDevice selectDevice(vk::Instance instance, vk::SurfaceKHR surface);
    static QueueFamilyIndices findQueueFamilies(vk::PhysicalDevice device, vk::SurfaceKHR surface);
};
