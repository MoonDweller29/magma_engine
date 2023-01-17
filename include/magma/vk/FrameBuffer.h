#pragma once
#include "vulkan/vulkan.hpp"
#include <vector>
#include "magma/vk/vulkan_common.h"

class FrameBuffer {
public:
    FrameBuffer(
            vk::Device device,
            const std::vector<vk::ImageView> &attachments,
            vk::RenderPass renderPass,
            vk::Extent2D extent);
    FrameBuffer(FrameBuffer&& other);
    NON_COPYABLE(FrameBuffer);
    ~FrameBuffer();

    vk::Framebuffer getFrameBuf() const { return _frameBuffer; }

private:
    vk::Framebuffer _frameBuffer;
    vk::Device      _device;
};