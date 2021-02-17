#include "magma/vk/swapChain.h"

#include <cstdint> // Necessary for UINT32_MAX
#include <algorithm>//for clamp
#include <iostream>
#include <sstream>

#include "magma/vk/vulkan_common.h"

static std::string to_string(const vk::SurfaceFormatKHR format) {
    return to_string(format.format) + " | " + to_string(format.colorSpace);
}

static std::string formatsToStr(const std::vector<vk::SurfaceFormatKHR> formats) {
    std::stringstream ss;
    ss << "format count: " << formats.size();

    int i = 0;
    for (const auto &format : formats) {
        ss << std::endl << "\t" << i << ": " << to_string(format);
        ++i;
    }

    return ss.str();
}

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {

    LOG_INFO("Available ", formatsToStr(availableFormats));

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            LOG_INFO("Selected Format: ", to_string(availableFormat));
            return availableFormat;
        }
    }

    VkSurfaceFormatKHR defaultFormat = availableFormats[0]; //not the best choice. But it's ok for now.
    LOG_INFO("Required format not found, fallback to default: ", to_string(defaultFormat));

    return defaultFormat;
}

static std::string presentModesToStr(const std::vector<vk::PresentModeKHR> &presentModes) {
    std::stringstream ss;
    ss << "present modes count: " << presentModes.size();

    int i = 0;
    for (const auto &mode : presentModes) {
        ss << std::endl << "\t" << i << ": " << to_string(mode);
        ++i;
    }

    return ss.str();
}

// should be changed to turn on vertical sync
VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    LOG_INFO("Available ", presentModesToStr(availablePresentModes));

    vk::PresentModeKHR requiredMode = vk::PresentModeKHR::eMailbox;

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == requiredMode) {
            LOG_INFO("Selected present mode: ", to_string(availablePresentMode));
            return (VkPresentModeKHR) availablePresentMode;
        }
    }

    vk::PresentModeKHR defaultMode = vk::PresentModeKHR::eFifo;
    LOG_INFO("Failed to find required present mode, fallback present mode: ", to_string(defaultMode));

    return (VkPresentModeKHR) defaultMode;
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

uint32_t chooseImageCount(const SwapChainSupportInfo &swapChainSupport)
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

SwapChain::SwapChain(LogicalDevice &device, const Window &window):
    device(device)
{
    SwapChainSupportInfo swapChainSupport = querySwapChainSupport(device.getVkPhysDevice(), window.getSurface());

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


    QueueFamilyIndices indices = device.physDevice().getQueueFamilyInds();
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

    createInfo.preTransform = (VkSurfaceTransformFlagBitsKHR) swapChainSupport.capabilities.currentTransform;
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
        std::vector<VkImageView> currImage = { imageViews[i].getView(), depthImageView };
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
