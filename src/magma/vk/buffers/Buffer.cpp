#include "magma/vk/buffers/Buffer.h"

Buffer::Buffer(VkBuffer buffer, VkDeviceMemory memory, BufferInfo* info)
    : _buffer(buffer),
    _bufferMemory(memory),
    _info(info)
{}