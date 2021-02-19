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
#include "magma/vk/commandBuffer.h"
#include "magma/vk/buffers/Buffer.h"

class LogicalDevice;

class BufferManager {
public:
    BufferManager(LogicalDevice &device);
    ~BufferManager();

    bool bufferExists(const std::string &name) const;
    Buffer& getBuffer(const std::string &name);

    Buffer& createBuffer(const std::string &name, VkDeviceSize size,
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    Buffer& createHostBuffer(const std::string &name, VkDeviceSize size, VkBufferUsageFlags usage);
    Buffer& createDeviceBuffer(const std::string &name, VkDeviceSize size, VkBufferUsageFlags usage);
    Buffer& createUniformBuffer(const std::string &name, VkDeviceSize size);

    template<class T>
    Buffer& createBufferWithData(const std::string &name, const std::vector<T> &data, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    Buffer& createBufferWithData(const std::string &name, const void* data, VkDeviceSize dataSize, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    
    template<class T>
    void copyDataToBuffer(Buffer &buffer, const std::vector<T> &data);
    void copyDataToBuffer(Buffer &buffer, const void* data, VkDeviceSize dataSize);
    void copyDataToHostBuffer(Buffer &buffer, const void* data, VkDeviceSize dataSize);
    void copyDataToDeviceBuffer(Buffer &buffer, const void* data, VkDeviceSize dataSize);

    void deleteBuffer(Buffer &buffer);
private:
    LogicalDevice &_device;
    CommandBufferArr _commandBuffers;
    std::unordered_map<std::string, Buffer> _buffers;

    void copyBufferToBuffer(Buffer &srcBuffer, Buffer &dstBuffer);
};

template<class T>
Buffer& BufferManager::createBufferWithData(const std::string &name, const std::vector<T> &data, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    if (bufferExists(name)) {
        throw std::invalid_argument("BufferManager::createBufferWithData buffer exist");
    }

    VkDeviceSize dataSize = sizeof(data[0]) * data.size();
    createBufferWithData(name, data.data(), dataSize, usage, properties);
}

template<class T>
void BufferManager::copyDataToBuffer(Buffer &buffer, const std::vector<T> &data) {
    VkDeviceSize dataSize = sizeof(data[0]) * data.size();
    VkDeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);
    copyDataToBuffer(buffer, data.data(), bufferSize);
}