#include "magma/vk/CmdSync.h"
#include "magma/vk/vulkan_common.h"

CmdSync::CmdSync(VkDevice device) : _isResourceOwner(true), _device(device) {
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

CmdSync::CmdSync(CmdSync &other):
    _isResourceOwner(false), _device(other._device),
    _semaphore(other._semaphore), _fence(other._fence)
{}

CmdSync::CmdSync(CmdSync &&other):
    _isResourceOwner(true), _device(other._device),
    _semaphore(other._semaphore), _fence(other._fence)
{
    other._isResourceOwner = false;
}

CmdSync::~CmdSync() {
    if (_isResourceOwner) {
        vkDestroySemaphore(_device, _semaphore, nullptr);
        vkDestroyFence(_device, _fence, nullptr);
    }
}

void CmdSync::resetFence() {
    if (_isResourceOwner) {
        vkResetFences(_device, 1, &_fence);
    } else {
        LOG_AND_THROW std::logic_error("attempt to reset fence from a non owner of CmdSync");
    }
}

bool CmdSync::isFenceSignaled() {
    return vkGetFenceStatus(_device, _fence) == VK_SUCCESS;
}
