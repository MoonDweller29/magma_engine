/**
 * @file BifferManager.h
 * @brief Tool for buffers creation and control
 * @version 0.1
 * @date 2021-02-18
 */
#pragma once
#include <string>
#include <unordered_map>

#include "magma/app/log.hpp"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/buffers/Buffer.h"

class LogicalDevice;

class BufferManager {
public:
    BufferManager(LogicalDevice &device);
    ~BufferManager();

    bool bufferExists(const std::string &name) const;
    Buffer& getBuffer(const std::string &name);

    Buffer& createBuffer(const std::string &name, vk::DeviceSize size,
        vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    Buffer& createStagingBuffer(const std::string &name, vk::DeviceSize size, vk::BufferUsageFlags usage);
    Buffer& createDeviceBuffer(const std::string &name, vk::DeviceSize size, vk::BufferUsageFlags usage);
    Buffer& createUniformBuffer(const std::string &name, vk::DeviceSize size);
    template<class T>
    Buffer& createVertexBuffer(const std::string &name, const std::vector<T> &data);
    template<class T>
    Buffer& createIndexBuffer(const std::string &name, const std::vector<T> &data);

    template<class T>
    Buffer& createBufferWithData(const std::string &name, const std::vector<T> &data, 
        vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    Buffer& createBufferWithData(const std::string &name, const void* data, vk::DeviceSize dataSize, 
        vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
    
    template<class T>
    void copyDataToBuffer(Buffer &buffer, const std::vector<T> &data);
    void copyDataToBuffer(Buffer &buffer, const void* data, vk::DeviceSize dataSize);

    void deleteBuffer(Buffer &buffer);

private:
    LogicalDevice &_device;
    CommandBuffer _commandBuffer;
    std::unordered_map<std::string, Buffer> _buffers;

    void copyDataToStagingBuffer(Buffer &buffer, const void* data, vk::DeviceSize dataSize);
    void copyDataToDeviceBuffer(Buffer &buffer, const void* data, vk::DeviceSize dataSize);
    void copyBufferToBuffer(Buffer &srcBuffer, Buffer &dstBuffer);
};

#include "BufferManager.hpp"