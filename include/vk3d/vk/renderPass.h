#pragma once
#include <vulkan/vulkan.h>

class RenderPass
{
    VkDevice device;
    VkRenderPass renderPass;
public:
    RenderPass(VkDevice device, VkPhysicalDevice physicalDevice, VkFormat imageFormat);
    VkRenderPass getHandler() const { return renderPass; }
    ~RenderPass();
};