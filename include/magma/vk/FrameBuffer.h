#pragma once
#include "vulkan/vulkan.hpp"
#include <vector>

class FrameBuffer {
public:
    FrameBuffer(
            VkDevice device,
            const std::vector<VkImageView> &attachments,
            VkRenderPass renderPass,
            VkExtent2D extent);
    FrameBuffer(FrameBuffer&& other);
    FrameBuffer(const FrameBuffer &other) = delete;
    ~FrameBuffer();

    VkFramebuffer getFrameBuf() const { return _frameBuffer; }

private:
    VkFramebuffer _frameBuffer;
    VkDevice      _device = VK_NULL_HANDLE;
};