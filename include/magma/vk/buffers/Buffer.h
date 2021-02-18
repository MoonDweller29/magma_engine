#pragma once
#include "magma/vk/buffers/BufferManager.h"
#include <string>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct BufferInfo {
    VkDevice            device;
    VkBufferCreateInfo  bufferInfo;
    std::string         name;
};

class Buffer {
friend class BufferManager;

public:
    Buffer() = default;
    const VkBuffer          &getBuffer()    const { return _buffer;         }
    const VkDeviceMemory    &getMemory()    const { return _bufferMemory;   }
    BufferInfo*             getInfo()       const { return _info;           }
private:
    Buffer(VkBuffer buffer, VkDeviceMemory memory, BufferInfo* info);

    VkBuffer _buffer;
    VkDeviceMemory _bufferMemory;
    BufferInfo* _info;
};
