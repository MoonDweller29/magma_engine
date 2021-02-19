/**
 * @file Buffer.h
 * @brief Class containig all information about buffer
 * @version 0.1
 * @date 2021-02-19
 */
#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>

struct BufferInfo {
    VkDevice                device;
    VkBufferCreateInfo      bufferInfo;
    VkMemoryPropertyFlags   memoryProperty;
    std::string             name;
};

class Buffer {
friend class BufferManager;

public:
    Buffer() = default;
    const VkBuffer          &getBuf()   const { return _buffer;         }
    const VkDeviceMemory    &getMem()   const { return _bufferMemory;   }
    BufferInfo*             getInfo()   const { return _info;           }
private:
    Buffer(VkBuffer buffer, VkDeviceMemory memory, BufferInfo* info);

    VkBuffer _buffer;
    VkDeviceMemory _bufferMemory;
    BufferInfo* _info;
};
