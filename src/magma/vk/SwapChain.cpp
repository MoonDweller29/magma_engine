#include "magma/vk/SwapChain.h"

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

vk::SurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats) {
    LOG_INFO("Available ", formatsToStr(availableFormats));

    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
            availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            LOG_INFO("Selected Format: ", to_string(availableFormat));
            return availableFormat;
        }
    }

    vk::SurfaceFormatKHR defaultFormat = availableFormats[0]; //not the best choice. But it's ok for now.
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
vk::PresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes) {
    LOG_INFO("Available ", presentModesToStr(availablePresentModes));

    vk::PresentModeKHR requiredMode = vk::PresentModeKHR::eMailbox;

    for (const auto& availablePresentMode : availablePresentModes) {
        if (availablePresentMode == requiredMode) {
            LOG_INFO("Selected present mode: ", to_string(availablePresentMode));
            return availablePresentMode;
        }
    }

    vk::PresentModeKHR defaultMode = vk::PresentModeKHR::eFifo;
    LOG_INFO("Failed to find required present mode, fallback present mode: ", to_string(defaultMode));

    return defaultMode;
}

vk::Extent2D SwapChain::chooseSwapExtent(const vk::SurfaceCapabilitiesKHR &capabilities, vk::Extent2D actualExtent) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {
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

uint32_t SwapChain::chooseImageCount(const vk::SurfaceCapabilitiesKHR &capabilities) {
    uint32_t imageCount = capabilities.minImageCount + 1; //recommended
    if ((capabilities.maxImageCount > 0) && (imageCount > capabilities.maxImageCount)) {
        imageCount = capabilities.maxImageCount;
    }

    return imageCount;
}

SwapChain::SwapChain(LogicalDevice &device, const Window &window):
    _device(device.getDevice())
{
    SwapChainSupportInfo swapChainSupport = device.getPhysDevice().getSwapChainSupportInfo(window.getSurface());

    vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    vk::Extent2D resolution = chooseSwapExtent(swapChainSupport.capabilities, window.getResolution());
    uint32_t imageCount = chooseImageCount(swapChainSupport.capabilities);

    vk::SwapchainCreateInfoKHR createInfo;
    createInfo.surface = window.getSurface();
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = resolution;
    createInfo.imageArrayLayers = 1; //should be 2 for VR
    createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;


    QueueFamilyIndices indices = device.getPhysDevice().getQueueFamilyInds();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE; //should be turned off to enable reading from backbuffer

    vk::Result result;
    std::tie(result, _swapChain) = _device.createSwapchainKHR(createInfo);
    VK_HPP_CHECK_ERR(result, "failed to create swap chain!");

    _imageFormat = surfaceFormat.format;
    _extent = resolution;
    acquireImages();
    createImageViews();
}

void SwapChain::acquireImages() {
    vk::Result result;
    std::tie(result, _images) = _device.getSwapchainImagesKHR(_swapChain);
    VK_HPP_CHECK_ERR(result, "failed to acquire swap chain images");
}

void SwapChain::createImageViews() {
    for (uint32_t i = 0; i < _images.size(); i++) {
        _imageViews.emplace_back(_device, _images[i], _imageFormat, vk::ImageAspectFlagBits::eColor);
    }
}

void SwapChain::createFrameBuffers(vk::RenderPass renderPass, vk::ImageView depthImageView) {
    for (size_t i = 0; i < _imageViews.size(); i++) {
        std::vector<vk::ImageView> currImage = { _imageViews[i].getView(), depthImageView };
        _frameBuffers.emplace_back(_device, currImage, renderPass, _extent);
    }
}
void SwapChain::clearFrameBuffers() {
    _frameBuffers.clear();
}

std::vector<VkFramebuffer> SwapChain::getVkFrameBuffers() const {
    std::vector<VkFramebuffer> buffers;
    for (size_t i = 0; i < _frameBuffers.size(); ++i) {
        buffers.push_back(_frameBuffers[i].getFrameBuf());
    }

    return buffers;
}

SwapChain::~SwapChain() {
    _device.destroySwapchainKHR(_swapChain);
}
