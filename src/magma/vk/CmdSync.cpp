#include "magma/vk/CmdSync.h"

#include <cstdint>
#include "magma/vk/vulkan_common.h"

CmdSync::CmdSync(vk::Device device, bool makeFenceSignaled) : _isResourceOwner(true), _device(device) {
    vk::SemaphoreCreateInfo semaphoreInfo;
    vk::Result result;
    std::tie(result, _semaphore) = _device.createSemaphore(semaphoreInfo);
    VK_CHECK_ERR(result, "failed to create semaphore!");

    vk::FenceCreateInfo fenceInfo;
    if (makeFenceSignaled) {
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    }

    std::tie(result, _fence) = _device.createFence(fenceInfo);
    VK_CHECK_ERR(result, "failed to create fence!");
}

CmdSync::CmdSync(CmdSync &other):
    _isResourceOwner(false), _device(other._device),
    _semaphore(other._semaphore), _fence(other._fence)
{}

CmdSync::CmdSync(CmdSync &&other):
    _isResourceOwner(other._isResourceOwner), _device(other._device),
    _semaphore(other._semaphore), _fence(other._fence)
{
    other._isResourceOwner = false;
}

CmdSync::~CmdSync() {
    if (_isResourceOwner) {
        _device.destroySemaphore(_semaphore);
        _device.destroyFence(_fence);
    }
}

void CmdSync::waitForFence() const {
    vk::Result result = _device.waitForFences(1, &_fence, VK_TRUE, UINT64_MAX);
}

void CmdSync::resetFence() {
    if (_isResourceOwner) {
        vk::Result result = _device.resetFences(1, &_fence);
        VK_CHECK_ERR(result, "failed to reset fence");
    } else {
        LOG_AND_THROW std::logic_error("attempt to reset fence from a non owner of CmdSync");
    }
}

bool CmdSync::isFenceSignaled() const {
    return _device.getFenceStatus(_fence) == vk::Result::eSuccess;
}
