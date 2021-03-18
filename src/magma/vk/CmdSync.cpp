#include "magma/vk/CmdSync.h"
#include "magma/vk/vulkan_common.h"

CmdSync::CmdSync():
    _device(VK_NULL_HANDLE), _semaphore(VK_NULL_HANDLE), _fence(VK_NULL_HANDLE)
{}

void CmdSync::create(VkDevice device) {
    clear();
    _device = device;

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &_semaphore);
    VK_CHECK_ERR(result, "failed to create semaphores!");

    result = vkCreateFence(device, &fenceInfo, nullptr, &_fence);
    VK_CHECK_ERR(result, "failed to create fences!");
}

CmdSync::CmdSync(VkDevice device) {
    _device = VK_NULL_HANDLE;
    create(device);
}

CmdSync::CmdSync(CmdSync &other):
    _device(VK_NULL_HANDLE), _semaphore(other._semaphore), _fence(other._fence)
{}

CmdSync::CmdSync(CmdSync &&other):
    _device(other._device), _semaphore(other._semaphore), _fence(other._fence)
{
    other._device = VK_NULL_HANDLE;
}

void CmdSync::clear() {
    if (_device != VK_NULL_HANDLE) {
        vkDestroySemaphore(_device, _semaphore, nullptr);
        vkDestroyFence(_device, _fence, nullptr);
    }
}

CmdSync::~CmdSync() {
    clear();
}