#include "magma/vk/buffers/Buffer.h"

Buffer::Buffer(vk::Buffer buffer, vk::DeviceMemory memory, BufferInfo* info)
        : _buffer(buffer),
        _bufferMemory(memory),
        _info(info)
{}
