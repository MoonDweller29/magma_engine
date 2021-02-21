#include "magma/vk/buffers/BufferManager.h"

#include <stdexcept>
#include <vulkan/vulkan_core.h>

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
    if (!bufferExists(name)) {
        LOG_AND_THROW std::invalid_argument(name + " buffer exist");
    }
    return _buffers.at(name);
};

Buffer& BufferManager::createBuffer(const std::string &name, VkDeviceSize size, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    if (bufferExists(name)) {
        LOG_AND_THROW std::invalid_argument(name + " buffer exist");
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
    info->bufferInfo = bufferInfo;
    info->memoryProperty = properties;
    info->name = name;

    _buffers.emplace(name, Buffer(buffer, bufferMemory, info));
    return getBuffer(name);
}

Buffer& BufferManager::createStagingBuffer(const std::string &name, VkDeviceSize size, VkBufferUsageFlags usage) {
    return createBuffer(name, size, usage, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

Buffer& BufferManager::createDeviceBuffer(const std::string &name, VkDeviceSize size, VkBufferUsageFlags usage) {
    return createBuffer(name, size, usage, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
}

Buffer& BufferManager::createUniformBuffer(const std::string &name, VkDeviceSize size) {
    return createStagingBuffer(name, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT);
}

Buffer& BufferManager::createBufferWithData(const std::string &name, const void* data, VkDeviceSize dataSize, 
        VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
    bool isDeviceBuffer = properties & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    if (isDeviceBuffer) {
        usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    }
    Buffer& buffer = createBuffer(name, dataSize, usage, properties);
    copyDataToBuffer(buffer, data, dataSize);
    return buffer;
}

void BufferManager::copyDataToBuffer(Buffer &buffer, const void* data, VkDeviceSize dataSize) {
    VkDeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);

    bool isDeviceBuffer = buffer.getInfo()->memoryProperty & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    if (isDeviceBuffer) {
        copyDataToDeviceBuffer(buffer, data, bufferSize);
    } else {
        copyDataToStagingBuffer(buffer, data, bufferSize);
    }
}

void BufferManager::copyDataToStagingBuffer(Buffer &buffer, const void* data, VkDeviceSize dataSize) {
    VkDeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);

    void* mapped_data_p;
    vkMapMemory(_device.handler(), buffer.getMem(), 0, bufferSize, 0, &mapped_data_p);
    {
        memcpy(mapped_data_p, data, (size_t)bufferSize);
    }
    vkUnmapMemory(_device.handler(), buffer.getMem());
}

void BufferManager::copyDataToDeviceBuffer(Buffer &buffer, const void* data, VkDeviceSize dataSize) {
    VkDeviceSize bufferSize = buffer.getInfo()->bufferInfo.size;
    if (dataSize != bufferSize) {
        LOG_WARNING("Buffers size ", bufferSize, " and data size ", dataSize, " not equal");
    }
    bufferSize = std::min(bufferSize, dataSize);

    Buffer& stagingBuffer = createStagingBuffer("stagingBuffer", bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    void* mapped_data_p;
    vkMapMemory(_device.handler(), stagingBuffer.getMem(), 0, bufferSize, 0, &mapped_data_p);
    {
        memcpy(mapped_data_p, data, (size_t)bufferSize);
    }
    vkUnmapMemory(_device.handler(), stagingBuffer.getMem());

    copyBufferToBuffer(stagingBuffer, buffer);

    deleteBuffer(stagingBuffer);
}

void BufferManager::deleteBuffer(Buffer &buffer) {
    _buffers.erase(buffer.getInfo()->name);
    delete buffer.getInfo();
    vkDestroyBuffer(_device.handler(), buffer.getBuf(), nullptr);
    vkFreeMemory(_device.handler(), buffer.getMem(), nullptr);
}

void BufferManager::copyBufferToBuffer(Buffer &srcBuffer, Buffer &dstBuffer) {
    if ((srcBuffer.getInfo()->bufferInfo.usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) == 0) {
        LOG_AND_THROW std::invalid_argument("srcBuffer has invalid usage");
    }
    if ((dstBuffer.getInfo()->bufferInfo.usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) == 0) {
        LOG_AND_THROW std::invalid_argument("dstBuffer has invalid usage");
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
        vkCmdCopyBuffer(cmdBuf, srcBuffer.getBuf(), dstBuffer.getBuf(), 1, &copyRegion);
    }
    _commandBuffers.endAndSubmitCmdBuf_sync(0, _device.getGraphicsQueue());
}
