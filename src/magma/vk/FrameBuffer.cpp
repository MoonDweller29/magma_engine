#include "magma/vk/FrameBuffer.h"

#include "magma/vk/vulkan_common.h"

FrameBuffer::FrameBuffer(
        VkDevice device,
        const std::vector<VkImageView> &attachments,
        VkRenderPass renderPass,
        VkExtent2D extent
) : _device(device)
{
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &_frameBuffer);
    VK_CHECK_ERR(result, "failed to create framebuffer!");
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) :
    _frameBuffer(other._frameBuffer), _device(other._device)
{
    other._device = VK_NULL_HANDLE;
}

FrameBuffer::~FrameBuffer() {
    if (_device != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(_device, _frameBuffer, nullptr);
    }
}
