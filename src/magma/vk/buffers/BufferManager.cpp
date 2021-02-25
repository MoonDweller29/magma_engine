#include "magma/vk/buffers/BufferManager.h"

#include <stdexcept>

#include "magma/vk/LogicalDevice.h"
#include "magma/vk/vulkan_common.h"
#include "magma/vk/buffers/Buffer.h"


BufferManager::BufferManager(LogicalDevice &device) 
        : _device(device),
        _commandBuffer(device.c_getDevice(), device.getGraphicsCmdPool())
{}

BufferManager::~BufferManager() {
    if (_buffers.size() > 0) {
        LOG_WARNING(_buffers.size(), " buffers haven't been removed");
        while(_buffers.size() > 0) {
            deleteBuffer(_buffers.begin()->second);
        }
    }
}

bool BufferManager::bufferExists(const std::string &name) const {
    return _buffers.find(name) != _buffers.end();
}

Buffer& BufferManager::getBuffer(const std::string &name) {
    if (!bufferExists(name)) {
        LOG_AND_THROW std::invalid_argument(name + " buffer exist");
    }
    return _buffers.at(name);
};

Buffer& BufferManager::createBuffer(const std::string &name, vk::DeviceSize size, 
        vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
    if (bufferExists(name)) {
        LOG_AND_THROW std::invalid_argument(name + " buffer exist");
    }
    vk::BufferCreateInfo bufferInfo;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = vk::SharingMode::eExclusive;

    auto [result, buffer] = _device.getDevice().createBuffer(bufferInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create buffer!");

    vk::MemoryRequirements memRequirements = _device.getDevice().getBufferMemoryRequirements(buffer);

    VkDeviceMemory bufferMemory  = _device.memAlloc(memRequirements, properties);

    result = _device.getDevice().bindBufferMemory(buffer, bufferMemory, 0);
    VK_HPP_CHECK_ERR(result, "Failed to bind buffer!");

    BufferInfo* info = new BufferInfo;
    info->device = _device.c_getDevice();
    info->bufferInfo = bufferInfo;
    info->memoryProperty = properties;
    info->name = name;

    _buffers.emplace(name, Buffer(buffer, bufferMemory, info));
    return getBuffer(name);
}

Buffer& BufferManager::createStagingBuffer(const std::string &name, vk::DeviceSize size, vk::BufferUsageFlags usage) {
    return createBuffer(name, size, usage, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
}

Buffer& BufferManager::createDeviceBuffer(const std::string &name, vk::DeviceSize size, vk::BufferUsageFlags usage) {
    return createBuffer(name, size, usage, vk::MemoryPropertyFlagBits::eDeviceLocal);
}

Buffer& BufferManager::createUniformBuffer(const std::string &name, vk::DeviceSize size) {
    return createStagingBuffer(name, size, vk::BufferUsageFlagBits::eUniformBuffer);
}

Buffer& BufferManager::createBufferWithData(const std::string &name, const void* data, vk::DeviceSize dataSize, 
        vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties) {
    auto isDeviceBuffer = properties & vk::MemoryPropertyFlagBits::eDeviceLocal;
    if (isDeviceBuffer) {
        usage |= vk::BufferUsageFlagBits::eTransferDst;
    }
    Buffer& buffer = createBuffer(name, dataSize, usage, properties);
    copyDataToBuffer(buffer, data, dataSize);
    return buffer;
}

void BufferManager::copyDataToBuffer(Buffer &buffer, const void* data, vk::DeviceSize dataSize) {
    vk::DeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);

    auto isDeviceBuffer = buffer.getInfo()->memoryProperty & vk::MemoryPropertyFlagBits::eDeviceLocal;
    if (isDeviceBuffer) {
        copyDataToDeviceBuffer(buffer, data, bufferSize);
    } else {
        copyDataToStagingBuffer(buffer, data, bufferSize);
    }
}

void BufferManager::copyDataToStagingBuffer(Buffer &buffer, const void* data, vk::DeviceSize dataSize) {
    vk::DeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);

    auto [result, data_ptr] = _device.getDevice().mapMemory(buffer.getMem(), 0, bufferSize, {});
    {
        memcpy(data_ptr, data, (size_t)bufferSize);
    }
    _device.getDevice().unmapMemory(buffer.getMem());
}

void BufferManager::copyDataToDeviceBuffer(Buffer &buffer, const void* data, vk::DeviceSize dataSize) {
    vk::DeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);

    Buffer& stagingBuffer = createStagingBuffer(buffer.getInfo()->name + "_stagingBuffer",
        bufferSize, vk::BufferUsageFlagBits::eTransferSrc);

    auto [result, data_ptr] = _device.getDevice().mapMemory(stagingBuffer.getMem(), 0, bufferSize, {});
    {
        memcpy(data_ptr, data, (size_t)bufferSize);
    }
    _device.getDevice().unmapMemory(stagingBuffer.getMem());

    copyBufferToBuffer(stagingBuffer, buffer);
    deleteBuffer(stagingBuffer);
}

void BufferManager::deleteBuffer(Buffer &buffer) {
    _buffers.erase(buffer.getInfo()->name);
    delete buffer.getInfo();
    _device.getDevice().destroyBuffer(buffer.getBuf());
    _device.getDevice().freeMemory(buffer.getMem());
}

void BufferManager::copyBufferToBuffer(Buffer &srcBuffer, Buffer &dstBuffer) {
    if (!(srcBuffer.getInfo()->bufferInfo.usage & vk::BufferUsageFlagBits::eTransferSrc)) {
        LOG_AND_THROW std::invalid_argument("srcBuffer has invalid usage");
    }
    if (!(dstBuffer.getInfo()->bufferInfo.usage & vk::BufferUsageFlagBits::eTransferDst)) {
        LOG_AND_THROW std::invalid_argument("dstBuffer has invalid usage");
    }
    vk::DeviceSize bufferSize = std::min(srcBuffer.getInfo()->bufferInfo.size,
        dstBuffer.getInfo()->bufferInfo.size);

    _commandBuffer.reset();
    vk::CommandBuffer cmdBuf = _commandBuffer.begin();
    {
        vk::BufferCopy copyRegion;
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = bufferSize;

        cmdBuf.copyBuffer(srcBuffer.getBuf(), dstBuffer.getBuf(), copyRegion);
    }
    _commandBuffer.endAndSubmit_sync(_device.getGraphicsQueue());
}
