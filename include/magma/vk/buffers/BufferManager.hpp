/**
 * @file BufferManager.hpp
 * @brief Implementation of BufferManager template methods
 * @version 0.1
 * @date 2021-02-19
 */
#pragma once

template<class T>
Buffer& BufferManager::createBufferWithData(const std::string &name, const std::vector<T> &data, 
        vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
    vk::DeviceSize dataSize = sizeof(data[0]) * data.size();
    return createBufferWithData(name, data.data(), dataSize, usage, properties);
}

template<class T>
Buffer& BufferManager::createVertexBuffer(const std::string &name, const std::vector<T> &data) {
    return createBufferWithData(name, data, vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
}

template<class T>
Buffer& BufferManager::createIndexBuffer(const std::string &name, const std::vector<T> &data) {
    return createBufferWithData(name, data, vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal);
}

template<class T>
void BufferManager::copyDataToBuffer(Buffer &buffer, const std::vector<T> &data) {
    vk::DeviceSize dataSize = sizeof(data[0]) * data.size();
    vk::DeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffer size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);
    copyDataToBuffer(buffer, data.data(), bufferSize);
}
