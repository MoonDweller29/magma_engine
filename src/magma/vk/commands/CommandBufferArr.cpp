#include "magma/vk/commands/CommandBufferArr.h"

CommandBufferArr::CommandBufferArr(vk::Device device, vk::CommandPool commandPool, uint32_t count)
        : _device(device),
        _commandPool(commandPool) {
    vk::CommandBufferAllocateInfo allocInfo;
    allocInfo.commandPool = _commandPool;
    allocInfo.level = vk::CommandBufferLevel::ePrimary;
    allocInfo.commandBufferCount = count;

    vk::Result result;
    std::tie(result, _commandBuffers) = _device.allocateCommandBuffers(allocInfo);
    VK_CHECK_ERR(result, "Failed to allocate command buffers!");
}

CommandBufferArr::~CommandBufferArr() {
    _device.freeCommandBuffers(_commandPool, _commandBuffers.size(), _commandBuffers.data());
}

vk::CommandBuffer CommandBufferArr::begin(uint32_t i) {
    vk::CommandBufferBeginInfo beginInfo;
    vk::Result result = _commandBuffers[i].begin(beginInfo);
    VK_CHECK_ERR(result, "Failed to begin recording command buffer!");
    return _commandBuffers.at(i);
}

void CommandBufferArr::end(uint32_t i) {
    vk::Result result = _commandBuffers[i].end();
    VK_CHECK_ERR(result, "Failed to record command buffer!");
}

void CommandBufferArr::submit_sync(uint32_t i, vk::Queue queue) {
    vk::SubmitInfo submitInfo;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffers[i];

    vk::Result result = queue.submit(submitInfo, vk::Fence());
    VK_CHECK_ERR(result, "Failed to submit command buffer!");
    result = queue.waitIdle();
    VK_CHECK_ERR(result, "Failed to wait idle command buffer!");
}

void CommandBufferArr::endAndSubmit_sync(uint32_t i, vk::Queue queue) {
    end(i);
    submit_sync(i, queue);
}

void CommandBufferArr::reset(uint32_t i, vk::CommandBufferResetFlags flag) {
    _commandBuffers[i].reset(flag);
}
