#include "magma/vk/buffers/BufferManager.h"
template<class T>
Buffer& BufferManager::createBufferWithData(const std::string &name, const std::vector<T> &data, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    if (bufferExists(name)) {
        throw std::invalid_argument("BufferManager::createBufferWithData buffer exist");
    }

    VkDeviceSize dataSize = sizeof(data[0]) * data.size();
    return createBufferWithData(name, data.data(), dataSize, usage, properties);
}

template<class T>
Buffer& BufferManager::createVertexBuffer(const std::string &name, const std::vector<T> &data) {
    if (bufferExists(name)) {
        throw std::invalid_argument("BufferManager::createVertexBuffer buffer exist");
    }

    return createBufferWithData(name, data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

template<class T>
Buffer& BufferManager::createIndexBuffer(const std::string &name, const std::vector<T> &data) {
    if (bufferExists(name)) {
        throw std::invalid_argument("BufferManager::createIndexBuffer buffer exist");
    }

    return createBufferWithData(name, data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
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
