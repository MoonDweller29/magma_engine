#include "commandPool.h"
#include "vulkan_common.h"

CommandPool::CommandPool(uint32_t queueFamilyIndex, VkDevice device)
{
    this->device = device;
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndex;
    poolInfo.flags = 0; // Optional

    VkResult result = vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool);
    vk_check_err(result, "failed to create command pool!");
}

CommandPool::~CommandPool()
{
    vkDestroyCommandPool(device, commandPool, nullptr);
}