#pragma once
#include "physicalDevice.h"
#include "window.h"
#include <vulkan/vulkan.h>
#include <vector>

class SwapChain
{
    VkSwapchainKHR swapChain;
    VkDevice device;
    std::vector<VkImage> images;
    VkFormat imageFormat;
    VkExtent2D extent;

    void acquireImages();
public:
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D actualExtent);

    SwapChain(VkDevice device, const PhysicalDevice &physicalDevice, const Window &window);
    ~SwapChain();
};