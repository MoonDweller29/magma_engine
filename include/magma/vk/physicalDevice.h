#pragma once
#include <vulkan/vulkan.h>
#include <optional>
#include <vector>

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete();
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

bool checkDeviceExtensionSupport(VkPhysicalDevice device);
SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

class PhysicalDevice
{
    QueueFamilyIndices inds;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    static bool isSuitable(VkPhysicalDevice device, const VkSurfaceKHR &surface);
    static int rateSuitability(VkPhysicalDevice device, const VkSurfaceKHR &surface);
    static VkPhysicalDevice selectDevice(VkInstance instance, const VkSurfaceKHR &surface);
    static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, const VkSurfaceKHR &surface);

public:
    static const std::vector<const char*> extensions;

    PhysicalDevice(VkInstance instance, VkSurfaceKHR surface);

    QueueFamilyIndices getQueueFamilyInds() const { return inds; }
    VkPhysicalDevice device() const { return physicalDevice; }
    ~PhysicalDevice() = default;
};
