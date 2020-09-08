#pragma once
#include <vulkan/vulkan.h>
#include "physicalDevice.h"
#include "buffer.h"
#include "texture.h"
#include <vector>
#include <cstring>

class LogicalDevice
{
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool graphicsCmdPool;

    void acquireQueues(QueueFamilyIndices indices);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    template<class T>
    Buffer createStagedBuffer(const std::vector<T> &data, VkBufferUsageFlags usageFlags);
public:


    LogicalDevice(const PhysicalDevice &physicalDevice);
    VkDevice handler() const { return device; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    const VkCommandPool &getGraphicsCmdPool() const { return graphicsCmdPool; }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // buffer management
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    Buffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    template<class T>
    Buffer createVertexBuffer(const std::vector<T> &data);
    template<class T>
    Buffer createIndexBuffer(const std::vector<T> &data);
    Buffer createUniformBuffer(VkDeviceSize size);

    void deleteBuffer(Buffer &buffer);
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////


    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // texture management
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    Texture createTexture2D(
            uint32_t width, uint32_t height,
            VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    void deleteTexture(Texture &tex);

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    ~LogicalDevice();
};

#include "logicalDevice.hpp"