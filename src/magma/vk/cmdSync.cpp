#include "magma/vk/cmdSync.h"
#include "magma/vk/vulkan_common.h"

CmdSync::CmdSync():
    device(VK_NULL_HANDLE), semaphore(VK_NULL_HANDLE), fence(VK_NULL_HANDLE)
{}

void CmdSync::create(VkDevice device)
{
    clear();
    this->device = device;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore);
    vk_check_err(result, "failed to create semaphores!");

    result = vkCreateFence(device, &fenceInfo, nullptr, &fence);
    vk_check_err(result, "failed to create fences!");
}

CmdSync::CmdSync(VkDevice device)
{
    this->device = VK_NULL_HANDLE;
    create(device);
}

CmdSync::CmdSync(CmdSync &other):
    device(VK_NULL_HANDLE), semaphore(other.semaphore), fence(other.fence)
{}

CmdSync::CmdSync(CmdSync &&other):
    device(other.device), semaphore(other.semaphore), fence(other.fence)
{
    other.device = VK_NULL_HANDLE;
}

void CmdSync::clear()
{
    if (device != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(device, semaphore, nullptr);
        vkDestroyFence(device, fence, nullptr);
    }
}

CmdSync::~CmdSync()
{
    clear();
}
