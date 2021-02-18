#pragma once
#include <vulkan/vulkan.h>

#include <vector>
#include <cstring>
#include <memory>

#include "magma/vk/textures/TextureManager.h"
#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "magma/vk/buffer.h"

class LogicalDevice {
    PhysicalDevice _physDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool graphicsCmdPool;
    std::unique_ptr<TextureManager> _textureManager;

    void acquireQueues(QueueFamilyIndices indices);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    template<class T>
    Buffer createStagedBuffer(const std::vector<T> &data, VkBufferUsageFlags usageFlags);
public:


    LogicalDevice(const PhysicalDevice &physicalDevice, const std::vector<const char*> &deviceExtensions);
    VkDevice handler() const { return device; }
    VkPhysicalDevice getVkPhysDevice() const { return _physDevice.c_device(); }
    const PhysicalDevice &physDevice() const { return _physDevice; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    const VkCommandPool &getGraphicsCmdPool() const { return graphicsCmdPool; }
    TextureManager &getTextureManager() const { return *_textureManager; }

    VkDeviceMemory createDeviceMemory(VkMemoryRequirements memRequirements, VkMemoryPropertyFlags properties);

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

    ~LogicalDevice();
};

#include "logicalDevice.hpp"