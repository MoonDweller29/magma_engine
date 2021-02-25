#pragma once

#include <vulkan/vulkan.hpp>

#include "magma/vk/vulkan_common.h"

class SingleTimeCommandBuffer {
public:
    SingleTimeCommandBuffer(vk::Device device, vk::CommandPool commandPool);
    ~SingleTimeCommandBuffer();

    const vk::CommandBuffer getCmdBuf() const { return _commandBuffer; }
    void endAndSubmit_sync(vk::Queue queue);

private:
    vk::Device          _device;
    vk::CommandPool     _commandPool;
    vk::CommandBuffer   _commandBuffer;
};