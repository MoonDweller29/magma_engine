#pragma once
#include <vulkan/vulkan.hpp>

class CommandPool {
public:
    static vk::CommandPool createPool(vk::Device device, uint32_t queueFamilyIndex);

    CommandPool(vk::Device device, uint32_t queueFamilyIndex);
    ~CommandPool();

    vk::CommandPool getPool() const { return _commandPool; }

private:
    vk::CommandPool _commandPool;
    vk::Device      _device;
};