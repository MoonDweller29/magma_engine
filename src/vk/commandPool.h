#pragma once
#include <vulkan/vulkan.h>

class CommandPool
{
    VkCommandPool commandPool;
    VkDevice device;
public:
    CommandPool(VkDevice device, uint32_t queueFamilyIndex); //can be required from phys device
    ~CommandPool();

    static VkCommandPool createPool(VkDevice device, uint32_t queueFamilyIndex);
    VkCommandPool getHandler() const { return commandPool; }
};