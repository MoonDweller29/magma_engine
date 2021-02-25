#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/Window.h"
#include "magma/vk/FrameBuffer.h"
#include "magma/vk/textures/CustomImageView.h"

class SwapChain {
public:
    static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> &availableFormats);
    static VkPresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> &availablePresentModes);
    static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, VkExtent2D actualExtent);

    SwapChain(LogicalDevice &device, const Window &window);
    ~SwapChain();

    VkSwapchainKHR getSwapChain()   const { return _swapChain;          }
    VkFormat       getImageFormat() const { return _imageFormat;        }
    VkExtent2D     getExtent()      const { return _extent;             }
    size_t         imgCount()       const { return _imageViews.size();  }

    /// @todo remove framebuffers from swapChain
    std::vector<VkFramebuffer> getVkFrameBuffers() const;
    void createFrameBuffers(VkRenderPass renderPass, VkImageView depthImageView);
    void clearFrameBuffers();

private:
    VkSwapchainKHR                _swapChain;
    LogicalDevice                &_device;
    std::vector<VkImage>          _images;
    std::vector<CustomImageView>  _imageViews;
    std::vector<FrameBuffer>      _frameBuffers;
    VkFormat                      _imageFormat;
    VkExtent2D                    _extent;

    void acquireImages();
    void createImageViews();
};