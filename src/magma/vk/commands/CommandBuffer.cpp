#include "magma/vk/commands/CommandBuffer.h"

CommandBuffer::CommandBuffer(vk::Device device, vk::CommandPool commandPool)
        : _device(device),
        _commandPool(commandPool) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    vk::Result result = _device.allocateCommandBuffers(&allocInfo, &_commandBuffer);
    VK_CHECK_ERR(result, "Failed to allocate command buffers!");
}

CommandBuffer::~CommandBuffer() {
    _device.freeCommandBuffers(_commandPool, _commandBuffer);
}

vk::CommandBuffer CommandBuffer::begin(vk::CommandBufferUsageFlags flags) {
    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = flags;
    vk::Result result = _commandBuffer.begin(beginInfo);
    VK_CHECK_ERR(result, "Failed to begin recording command buffer!");
    return _commandBuffer;
}

void CommandBuffer::end() {
    vk::Result result = _commandBuffer.end();
    VK_CHECK_ERR(result, "Failed to record command buffer!");
}

void CommandBuffer::submit_sync(vk::Queue queue) {
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;

    vk::Result result = queue.submit(submitInfo, vk::Fence());
    VK_CHECK_ERR(result, "Failed to submit command buffer!");
    result = queue.waitIdle();
    VK_CHECK_ERR(result, "Failed to wait idle command buffer!");
}

void CommandBuffer::endAndSubmit_sync(vk::Queue queue) {
    end();
    submit_sync(queue);
}

void CommandBuffer::reset(vk::CommandBufferResetFlags flag) {
    _commandBuffer.reset(flag);
}
