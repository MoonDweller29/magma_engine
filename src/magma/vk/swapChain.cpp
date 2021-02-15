#include "magma/vk/swapChain.h"
#include "magma/vk/vulkan_common.h"
#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm>//for clamp
#include <iostream>

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
    for (const auto& availableFormat : availableFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return availableFormats[0]; //not the best choice. But it's ok for now.
}

// should be changed to turn on vertical sync
VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes)
{
    VkPresentModeKHR requiredMode = VK_PRESENT_MODE_MAILBOX_KHR;
//    VkPresentModeKHR required_mode = VK_PRESENT_MODE_IMMEDIATE_KHR;

    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == requiredMode) {
            return availablePresentMode;
        }
    }

    std::cout << "Failed to find required present mode\n";

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D actualExtent)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        return capabilities.currentExtent;
    } else
    {
//        VkExtent2D actualExtent = {WIDTH, HEIGHT};
        actualExtent.width = std::clamp(
                actualExtent.width,
                capabilities.minImageExtent.width,
                capabilities.maxImageExtent.width
        );
        actualExtent.height = std::clamp(
                actualExtent.height,
                capabilities.minImageExtent.height,
                capabilities.maxImageExtent.height
        );

        return actualExtent;
    }
}

uint32_t chooseImageCount(const SwapChainSupportDetails &swapChainSupport)
{
//    std::cout << "swap_chain minImageCount : " << swapChainSupport.capabilities.minImageCount << std::endl;
//    std::cout << "swap_chain maxImageCount : " << swapChainSupport.capabilities.maxImageCount << std::endl;
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1; //recommended
    if ((swapChainSupport.capabilities.maxImageCount > 0) &&
        (imageCount > swapChainSupport.capabilities.maxImageCount))
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    return imageCount;
}

SwapChain::SwapChain(LogicalDevice &device, const PhysicalDevice &physicalDevice, const Window &window):
    device(device)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice.device(), window.getSurface());

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D resolution = chooseSwapExtent(swapChainSupport.capabilities, window.getResolution());

    uint32_t imageCount = chooseImageCount(swapChainSupport);

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = window.getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = resolution;
    createInfo.imageArrayLayers = 1; //should be 2 for VR
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;


    QueueFamilyIndices indices = physicalDevice.getQueueFamilyInds();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT; //may be changed later
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; //should be turned off to enable reading from backbuffer
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult result = vkCreateSwapchainKHR(device.handler(), &createInfo, nullptr, &swapChain);
    VK_CHECK_ERR(result, "failed to create swap chain!");

    imageFormat = surfaceFormat.format;
    extent = resolution;
    acquireImages();
    createImageViews();
}

void SwapChain::acquireImages()
{
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device.handler(), swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device.handler(), swapChain, &imageCount, images.data());
}

void SwapChain::createImageViews()
{
    for (uint32_t i = 0; i < images.size(); i++)
    {
        imageViews.emplace_back(device.handler(), images[i], imageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }
}

void SwapChain::createFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView)
{
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        std::vector<VkImageView> currImage = { imageViews[i].getImageView(), depthImageView };
        frameBuffers.emplace_back(currImage, extent, renderPass, device.handler());
    }
}
void SwapChain::clearFrameBuffers()
{
    frameBuffers.clear();
}

std::vector<VkFramebuffer> SwapChain::getVkFrameBuffers() const
{
    std::vector<VkFramebuffer> buffers;
    for (size_t i = 0; i < frameBuffers.size(); ++i)
    {
        buffers.push_back(frameBuffers[i].getHandler());
    }

    return buffers;
}

SwapChain::~SwapChain()
{
    vkDestroySwapchainKHR(device.handler(), swapChain, nullptr);
}
