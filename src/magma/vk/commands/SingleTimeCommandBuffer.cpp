#include "magma/vk/commands/SingleTimeCommandBuffer.h"

SingleTimeCommandBuffer::SingleTimeCommandBuffer(vk::Device device, vk::CommandPool commandPool) 
        : _device(device), 
        _commandPool(commandPool) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = 1;

    vk::Result result = _device.allocateCommandBuffers(&allocInfo, &_commandBuffer);
    VK_HPP_CHECK_ERR(result, "Failed to allocate command buffers!");

    vk::CommandBufferBeginInfo beginInfo;
    beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
    result = _commandBuffer.begin(beginInfo);
    VK_HPP_CHECK_ERR(result, "Failed to begin recording command buffer!");
}

SingleTimeCommandBuffer::~SingleTimeCommandBuffer() {
    _device.freeCommandBuffers(_commandPool, _commandBuffer);
}

void SingleTimeCommandBuffer::endAndSubmit_sync(vk::Queue queue) {
    vk::Result result = _commandBuffer.end();
    VK_HPP_CHECK_ERR(result, "Failed to record command buffer!");

    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer;

    result = queue.submit(submitInfo, vk::Fence());
    VK_HPP_CHECK_ERR(result, "Failed to submit command buffer!");
    result = queue.waitIdle();
    VK_HPP_CHECK_ERR(result, "Failed to wait idle command buffer!");
}