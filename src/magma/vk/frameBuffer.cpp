#include "magma/vk/frameBuffer.h"
#include "magma/vk/vulkan_common.h"

FrameBuffer::FrameBuffer(
        const std::vector<VkImageView> &attachments,
        VkExtent2D extent,
        VkRenderPass renderPass,
        VkDevice device)
{
    this->device = device;
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = renderPass;
    framebufferInfo.attachmentCount = attachments.size();
    framebufferInfo.pAttachments = attachments.data();
    framebufferInfo.width = extent.width;
    framebufferInfo.height = extent.height;
    framebufferInfo.layers = 1;

    VkResult result = vkCreateFramebuffer(device, &framebufferInfo, nullptr, &frameBuffer);
    VK_CHECK_ERR(result, "failed to create framebuffer!");
}

FrameBuffer::FrameBuffer(FrameBuffer&& other)
{
    this->frameBuffer = other.frameBuffer;
    this->device = other.device;
    other.frameBuffer = VK_NULL_HANDLE;
}

FrameBuffer::~FrameBuffer()
{
    if (frameBuffer != VK_NULL_HANDLE)
        vkDestroyFramebuffer(device, frameBuffer, nullptr);
}
