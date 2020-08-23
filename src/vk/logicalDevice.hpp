#pragma once

template<class T>
Buffer LogicalDevice::createStagedBuffer(const std::vector<T> &data, VkBufferUsageFlags usageFlags)
{
    VkDeviceSize bufferSize = sizeof(data[0]) * data.size();

    Buffer stagingBuffer = createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    //filling the staging buffer
    void* mapped_data_p;
    vkMapMemory(device, stagingBuffer.mem, 0, bufferSize, 0, &mapped_data_p);
    {
        memcpy(mapped_data_p, data.data(), (size_t) bufferSize);
    }
    vkUnmapMemory(device, stagingBuffer.mem);

    Buffer vertexBuffer = createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | usageFlags,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
    );

    copyBuffer(stagingBuffer.buf, vertexBuffer.buf, bufferSize);

    vkDestroyBuffer(device, stagingBuffer.buf, nullptr);
    vkFreeMemory(device, stagingBuffer.mem, nullptr);

    return vertexBuffer;
}

template<class T>
Buffer LogicalDevice::createVertexBuffer(const std::vector<T> &data)
{
    return createStagedBuffer(data, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

template<class T>
Buffer LogicalDevice::createIndexBuffer(const std::vector<T> &data)
{
    return createStagedBuffer(data, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}