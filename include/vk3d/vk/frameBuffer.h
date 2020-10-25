#pragma once
#include "vulkan/vulkan.h"
#include <vector>

class FrameBuffer
{
    VkFramebuffer frameBuffer = VK_NULL_HANDLE;
    VkDevice device;
public:
    FrameBuffer(
            const std::vector<VkImageView> &attachments,
            VkExtent2D extent,
            VkRenderPass renderPass,
            VkDevice device);
    FrameBuffer(FrameBuffer&& other);
    FrameBuffer(const FrameBuffer &other) = delete;

    VkFramebuffer getHandler() const { return frameBuffer; }
    ~FrameBuffer();
};