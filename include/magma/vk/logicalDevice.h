#pragma once
#include <vulkan/vulkan.hpp>

#include <vector>
#include <cstring>
#include <memory>

#include "magma/vk/textures/TextureManager.h"
#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "magma/vk/buffers/BufferManager.h"

class LogicalDevice {
    PhysicalDevice _physDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool graphicsCmdPool;
    std::unique_ptr<TextureManager> _textureManager;
    std::unique_ptr<BufferManager>  _bufferManager;

    void acquireQueues(QueueFamilyIndices indices);
public:

    LogicalDevice(const PhysicalDevice &physicalDevice, const std::vector<const char*> &deviceExtensions);
    VkDevice handler() const { return device; }
    VkPhysicalDevice getVkPhysDevice() const { return _physDevice.c_device(); }
    const PhysicalDevice &getPhysDevice() const { return _physDevice; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    const VkCommandPool &getGraphicsCmdPool() const { return graphicsCmdPool; }
    TextureManager  &getTextureManager()    const { return *_textureManager;    }
    BufferManager   &getBufferManager()     const { return *_bufferManager;     }

    VkDeviceMemory createDeviceMemory(VkMemoryRequirements memRequirements, VkMemoryPropertyFlags properties);

    ~LogicalDevice();
};
