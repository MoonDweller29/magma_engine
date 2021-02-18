#include "magma/vk/buffers/Buffer.h"

#include "magma/vk/vulkan_common.h"

Buffer::Buffer(VkBuffer buffer, VkDeviceMemory memory, BufferInfo* info)
    : _buffer(buffer),
    _bufferMemory(memory),
    _info(info)
{}