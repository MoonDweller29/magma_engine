#include "magma/vk/commands/CommandPool.h"

#include "magma/vk/vulkan_common.h"

vk::CommandPool CommandPool::createPool(vk::Device device, uint32_t queueFamilyIndex) {
    vk::CommandPoolCreateInfo poolInfo(
        vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        queueFamilyIndex);

    auto [result, commandPool] = device.createCommandPool(poolInfo);
    VK_HPP_CHECK_ERR(result, "failed to create command pool!");

    return commandPool;
}

CommandPool::CommandPool(vk::Device device, uint32_t queueFamilyIndex) :
    _device(device),
    _commandPool(createPool(device, queueFamilyIndex))
{}

CommandPool::~CommandPool() {
    _device.destroyCommandPool(_commandPool);
}
