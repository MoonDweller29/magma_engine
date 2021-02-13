#pragma once
#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "logicalDevice.h"
#include "Window.h"
#include "frameBuffer.h"
#include <vulkan/vulkan.h>
#include <vector>

#include "magma/vk/textures/CustomImageView.h"

class SwapChain
{
    VkSwapchainKHR swapChain;
    LogicalDevice &device;
    std::vector<VkImage> images;
    std::vector<CustomImageView> imageViews;
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