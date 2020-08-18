#pragma once
#include <vulkan/vulkan.h>

class CommandPool
{
    VkCommandPool commandPool;
    VkDevice device;
public:
    CommandPool(uint32_t queueFamilyIndex, VkDevice device); //can be required from phys device
    ~CommandPool();

    VkCommandPool getHandler() const { return commandPool; }
};