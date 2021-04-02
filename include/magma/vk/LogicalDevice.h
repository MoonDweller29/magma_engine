#pragma once
#include <vulkan/vulkan.hpp>

#include <vector>
#include <cstring>
#include <memory>

#include "magma/vk/textures/TextureManager.h"
#include "magma/vk/physicalDevice/PhysicalDevice.h"
#include "magma/vk/buffers/BufferManager.h"
#include "magma/vk/vulkan_common.h"

struct Queue {
    vk::Queue queue;
    vk::CommandPool cmdPool;
    uint32_t queueFamily;
};

class LogicalDevice {
public:
    LogicalDevice(
            const PhysicalDevice              &physicalDevice,
            const vk::PhysicalDeviceFeatures  &physicalDeviceFeatures,
            const std::vector<const char*>    &deviceExtensions);
    NON_COPYABLE(LogicalDevice);
    ~LogicalDevice();

    vk::Device            getDevice()          const { return _device;                }
    const PhysicalDevice &getPhysDevice()      const { return _physDevice;            }

    const Queue          &getGraphicsQueue()   const { return _graphicsQueue;         }
    const Queue          &getPresentQueue()    const { return _presentQueue;          }

    TextureManager       &getTextureManager()  const { return *_textureManager;       }
    BufferManager        &getBufferManager()   const { return *_bufferManager;        }

    vk::DeviceMemory memAlloc(vk::MemoryRequirements memRequirements, vk::MemoryPropertyFlags properties);
    void waitIdle();

private:
    PhysicalDevice _physDevice;
    vk::Device     _device;

    Queue       _graphicsQueue;
    Queue       _presentQueue;

    std::unique_ptr<TextureManager> _textureManager;
    std::unique_ptr<BufferManager>  _bufferManager;

    void acquireQueues(QueueFamilyIndices indices);
};
