#pragma once
#include "physicalDevice.h"
#include "logicalDevice.h"
#include "window.h"
#include "frameBuffer.h"
#include <vulkan/vulkan.h>
#include <vector>

class SwapChain
{
    VkSwapchainKHR swapChain;
    LogicalDevice &device;
    std::vector<VkImage> images;
    std::vector<VkImageView> imageViews;
    std::vector<FrameBuffer> frameBuffers;
    VkFormat imageFormat;
    VkExtent2D extent;

    void acquireImages();
    void createImageViews();
public:
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D actualExtent);

    SwapChain(LogicalDevice &device, const PhysicalDevice &physicalDevice, const Window &window);
    ~SwapChain();

    VkSwapchainKHR getSwapChain() const { return swapChain; }
    VkFormat getImageFormat() const { return imageFormat; }
    VkExtent2D getExtent() const { return extent; }
    size_t imgCount() const { return imageViews.size(); }
    std::vector<VkFramebuffer> getVkFrameBuffers() const;
    void createFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView);
    void clearFrameBuffers();
};