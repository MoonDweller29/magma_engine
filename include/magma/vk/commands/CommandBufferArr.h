#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>

#include "magma/vk/vulkan_common.h"

class CommandBufferArr {
public:
    CommandBufferArr(vk::Device device, vk::CommandPool commandPool, uint32_t count);
    ~CommandBufferArr();

    uint32_t                    size()      const               { return _commandBuffers.size();    }
    const vk::CommandBuffer*    data()      const               { return _commandBuffers.data();    }
    const vk::CommandBuffer&    operator[] (const size_t index) { return _commandBuffers.at(index); }

    vk::CommandBuffer begin(uint32_t i);
    void end(uint32_t i);
    void submit_sync(uint32_t i, vk::Queue queue);
    void endAndSubmit_sync(uint32_t i, vk::Queue queue);
    void reset(uint32_t i, vk::CommandBufferResetFlags flag=vk::CommandBufferResetFlags());

private:
    vk::Device                      _device;
    vk::CommandPool                 _commandPool;
    std::vector<vk::CommandBuffer>  _commandBuffers;
};