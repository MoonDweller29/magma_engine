#pragma once
#include <vulkan/vulkan.h>

class CmdSync {
public:
    CmdSync(VkDevice device);
    CmdSync(CmdSync &other);
    CmdSync(CmdSync &&other);
    ~CmdSync();

    const VkSemaphore &getSemaphore() const { return _semaphore; }
    const VkFence     &getFence()     const { return _fence;     }
    void resetFence();
    bool isFenceSignaled();

private:
    bool        _isResourceOwner;
    VkDevice    _device;
    VkSemaphore _semaphore;
    VkFence     _fence;
};