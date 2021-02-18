/**
 * @file BifferManager.h
 * @brief Tool for buffers creation and control
 * @version 0.1
 * @date 2021-02-18
 */
#pragma once
#include <string>
#include <unordered_map>

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

    template<class T>
    Buffer& createBufferWithData(const std::string &name, const std::vector<T> &data, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    template<class T>
    void copyDataToBuffer(Buffer &buffer, const std::vector<T> &data);

    void deleteBuffer(Buffer &buffer);
private:
    LogicalDevice &_device;
    CommandBufferArr _commandBuffers;
    std::unordered_map<std::string, Buffer> _buffers;

    void copyBufferToBuffer(Buffer &srcBuffer, Buffer &dstBuffer);
};