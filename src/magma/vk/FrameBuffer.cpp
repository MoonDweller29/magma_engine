#include "magma/vk/FrameBuffer.h"

#include "magma/vk/vulkan_common.h"

FrameBuffer::FrameBuffer(
        vk::Device device,
        const std::vector<vk::ImageView> &attachments,
        vk::RenderPass renderPass,
        vk::Extent2D extent
) : _device(device)
{
    vk::FramebufferCreateInfo framebufferInfo;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    vk::Result result;
    std::tie(result, _frameBuffer) = _device.createFramebuffer(framebufferInfo);
    VK_HPP_CHECK_ERR(result, "failed to create framebuffer!");
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) :
    _frameBuffer(other._frameBuffer), _device(other._device)
{
    other._device = vk::Device();
}

FrameBuffer::~FrameBuffer() {
    if (_device) {
        _device.destroyFramebuffer(_frameBuffer);
    }
}
