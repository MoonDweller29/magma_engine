#include "magma/vk/commandPool.h"
#include "magma/vk/vulkan_common.h"

VkCommandPool CommandPool::createPool(VkDevice device, uint32_t queueFamilyIndex)
{
    VkCommandPool commandPool;

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndex;
    poolInfo.flags = 0; // Optional

    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    vk_check_err(result, "failed to create command pool!");

    return commandPool;
}


CommandPool::CommandPool(VkDevice device, uint32_t queueFamilyIndex)
{
    this->device = device;
    commandPool = createPool(device, queueFamilyIndex);
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device, commandPool, nullptr);
}
