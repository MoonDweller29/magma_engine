#pragma once
#include <vulkan/vulkan.hpp>

#include <vector>
#include <cstring>
#include <memory>

#include "magma/vk/textures/TextureManager.h"
#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "magma/vk/buffers/BufferManager.h"

class LogicalDevice {
public:
    LogicalDevice(
            const PhysicalDevice              &physicalDevice,
            const vk::PhysicalDeviceFeatures  &physicalDeviceFeatures,
            const std::vector<const char*>    &deviceExtensions);
    ~LogicalDevice();

    VkDevice              c_getDevice()        const { return VkDevice(_device); } //only for Vulkan C API
    vk::Device            getDevice()          const { return _device; }
    VkPhysicalDevice      getVkPhysDevice()    const { return _physDevice.c_device(); }
    const PhysicalDevice &getPhysDevice()      const { return _physDevice; }
    VkQueue               getGraphicsQueue()   const { return graphicsQueue; }
    VkQueue               getPresentQueue()    const { return presentQueue; }
    const VkCommandPool  &getGraphicsCmdPool() const { return graphicsCmdPool; }
    TextureManager       &getTextureManager()  const { return *_textureManager; }
    BufferManager        &getBufferManager()   const { return *_bufferManager; }

    vk::DeviceMemory memAlloc(vk::MemoryRequirements memRequirements, vk::MemoryPropertyFlags properties);

private:
    PhysicalDevice _physDevice;
    vk::Device     _device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool graphicsCmdPool;
    std::unique_ptr<TextureManager> _textureManager;
    std::unique_ptr<BufferManager>  _bufferManager;

    void acquireQueues(QueueFamilyIndices indices);
};
