/**
 * @file BufferManager.hpp
 * @brief Template methods implementation BufferManager
 * @version 0.1
 * @date 2021-02-19
 */
#pragma once

template<class T>
Buffer& BufferManager::createBufferWithData(const std::string &name, const std::vector<T> &data, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    VkDeviceSize dataSize = sizeof(data[0]) * data.size();
    return createBufferWithData(name, data.data(), dataSize, usage, properties);
}

template<class T>
Buffer& BufferManager::createVertexBuffer(const std::string &name, const std::vector<T> &data) {
    return createBufferWithData(name, data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

template<class T>
Buffer& BufferManager::createIndexBuffer(const std::string &name, const std::vector<T> &data) {
    return createBufferWithData(name, data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

template<class T>
void BufferManager::copyDataToBuffer(Buffer &buffer, const std::vector<T> &data) {
    VkDeviceSize dataSize = sizeof(data[0]) * data.size();
    VkDeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffer size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);
    copyDataToBuffer(buffer, data.data(), bufferSize);
}
