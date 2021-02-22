/**
 * @file Buffer.h
 * @brief Class containig all information about buffer
 * @version 0.1
 * @date 2021-02-19
 */
#pragma once
#include <string>
#include <vulkan/vulkan.hpp>

struct BufferInfo {
    vk::Device              device;
    vk::BufferCreateInfo    bufferInfo;
    vk::MemoryPropertyFlags memoryProperty;
    std::string             name;
};

class Buffer {
    friend class BufferManager;

public:
    Buffer() = default;
    vk::Buffer          getBuf()    const { return _buffer;         }
    vk::DeviceMemory    getMem()    const { return _bufferMemory;   }
    const BufferInfo*   getInfo()   const { return _info;           }

    [[deprecated]] VkBuffer         c_getBuf()  const { return (VkBuffer)_buffer;               }
    [[deprecated]] VkDeviceMemory   c_getMem()  const { return (VkDeviceMemory)_bufferMemory;   }

private:
    Buffer(vk::Buffer buffer, vk::DeviceMemory memory, BufferInfo* info);

    vk::Buffer        _buffer;
    vk::DeviceMemory  _bufferMemory;
    BufferInfo*       _info;
};
