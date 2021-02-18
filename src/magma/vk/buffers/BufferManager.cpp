#include "magma/vk/buffers/BufferManager.h"

#include <stdexcept>
#include <vulkan/vulkan_core.h>

#include "magma/app/log.hpp"
#include "magma/vk/logicalDevice.h"
#include "magma/vk/vulkan_common.h"
#include "magma/vk/buffers/Buffer.h"


BufferManager::BufferManager(LogicalDevice &device) 
        : _device(device),
        _commandBuffers(device.handler(), device.getGraphicsCmdPool(), 1)
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
    if (bufferExists(name)) {
        return _buffers.at(name);
    } else {
        throw std::invalid_argument("BufferManager::getBuffer buffer not exist");
    }
};

Buffer& BufferManager::createBuffer(const std::string &name, VkDeviceSize size, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    if (bufferExists(name)) {
        throw std::invalid_argument("BufferManager::createBuffer buffer exist");
    }
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer buffer;
    VkResult result = vkCreateBuffer(_device.handler(), &bufferInfo, nullptr, &buffer);
    VK_CHECK_ERR(result, "failed to create buffer!");

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(_device.handler(), buffer, &memRequirements);

    VkDeviceMemory bufferMemory  = _device.createDeviceMemory(memRequirements, properties);

    vkBindBufferMemory(_device.handler(), buffer, bufferMemory, 0);

    BufferInfo* info = new BufferInfo;
    info->device = _device.handler();
    info->name = name;
    info->bufferInfo = bufferInfo;

    _buffers.emplace(name, Buffer(buffer, bufferMemory, info));
    return getBuffer(name);
}

template<class T>
Buffer& BufferManager::createBufferWithData(const std::string &name, const std::vector<T> &data, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    if (bufferExists(name)) {
        throw std::invalid_argument("BufferManager::createBufferWithData buffer exist");
    }

    VkDeviceSize bufferSize = sizeof(data[0]) * data.size();
    Buffer& buffer = createBuffer(name, bufferSize, usage, properties);
    copyDataToBuffer(buffer, data);
    return buffer;
}

template<class T>
void BufferManager::copyDataToBuffer(Buffer &buffer, const std::vector<T> &data) {
    VkDeviceSize dataSize = sizeof(data[0]) * data.size();
    VkDeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);

    Buffer& stagingBuffer = createBuffer("stagingBuffer", 
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* mapped_data_p;
    vkMapMemory(_device.handler(), stagingBuffer.getMemory(), 0, bufferSize, 0, &mapped_data_p);
    {
        memcpy(mapped_data_p, data.data(), (size_t) bufferSize);
    }
    vkUnmapMemory(_device.handler(), stagingBuffer.getMemory());

    copyBufferToBuffer(stagingBuffer, buffer);

    deleteBuffer(stagingBuffer);
}

void BufferManager::deleteBuffer(Buffer &buffer) {
    _buffers.erase(buffer.getInfo()->name);
    delete buffer.getInfo();
    vkDestroyBuffer(_device.handler(), buffer.getBuffer(), nullptr);
    vkFreeMemory(_device.handler(), buffer.getMemory(), nullptr);
}

void BufferManager::copyBufferToBuffer(Buffer &srcBuffer, Buffer &dstBuffer) {
    if ((srcBuffer.getInfo()->bufferInfo.usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) == 0) {
        throw std::invalid_argument("BufferManager::srcBuffer have unvalid usage");
    }
    if ((dstBuffer.getInfo()->bufferInfo.usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) == 0) {
        throw std::invalid_argument("BufferManager::dstBuffer have unvalid usage");
    }
    VkDeviceSize bufferSize = std::min(srcBuffer.getInfo()->bufferInfo.size,
        dstBuffer.getInfo()->bufferInfo.size);

    _commandBuffers.resetCmdBuf(0);
    VkCommandBuffer cmdBuf = _commandBuffers.beginCmdBuf(0);
    {
        VkBufferCopy copyRegion{};
        copyRegion.srcOffset = 0;
        copyRegion.dstOffset = 0;
        copyRegion.size = bufferSize;
        vkCmdCopyBuffer(cmdBuf, srcBuffer.getBuffer(), dstBuffer.getBuffer(), 1, &copyRegion);
    }
    _commandBuffers.endAndSubmitCmdBuf_sync(0, _device.getGraphicsQueue());
}
