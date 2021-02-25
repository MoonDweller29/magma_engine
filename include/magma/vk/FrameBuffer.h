#pragma once
#include "vulkan/vulkan.hpp"
#include <vector>

class FrameBuffer {
public:
    FrameBuffer(
            vk::Device device,
            const std::vector<vk::ImageView> &attachments,
            vk::RenderPass renderPass,
            vk::Extent2D extent);
    FrameBuffer(FrameBuffer&& other);
    FrameBuffer(const FrameBuffer &other) = delete;
    ~FrameBuffer();

    VkFramebuffer getFrameBuf() const { return _frameBuffer; }

private:
    vk::Framebuffer _frameBuffer;
    vk::Device      _device;
};