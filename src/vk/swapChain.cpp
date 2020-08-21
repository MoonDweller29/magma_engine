#include "swapChain.h"
#include "vulkan_common.h"
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
    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

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

SwapChain::SwapChain(VkDevice device, const PhysicalDevice &physicalDevice, const Window &window)
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

    this->device = device;
    VkResult result = vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain);
    vk_check_err(result, "failed to create swap chain!");

    imageFormat = surfaceFormat.format;
    extent = resolution;
    acquireImages();
    createImageViews();
}

void SwapChain::acquireImages()
{
    uint32_t imageCount;
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, swapChain, &imageCount, images.data());
}

void SwapChain::createImageViews()
{
    size_t imgCount = images.size();
    imageViews.resize(imgCount);
    for (size_t i = 0; i < imgCount; ++i)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = images[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = imageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        VkResult result = vkCreateImageView(device, &createInfo, nullptr, &imageViews[i]);
        vk_check_err(result, "failed to create image views!");
    }
}

void SwapChain::createFrameBuffers(VkRenderPass renderPass)
{
    for (size_t i = 0; i < imageViews.size(); i++)
    {
        std::vector<VkImageView> currImage = { imageViews[i] };
        frameBuffers.emplace_back(currImage, extent, renderPass, device);
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
    for (auto imageView : imageViews)
    {
        vkDestroyImageView(device, imageView, nullptr);
    }
    vkDestroySwapchainKHR(device, swapChain, nullptr);
}