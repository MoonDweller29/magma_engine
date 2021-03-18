#pragma once
#include <vulkan/vulkan.h>

class CmdSync {
public:
    CmdSync();
    CmdSync(VkDevice device);
    CmdSync(CmdSync &other);
    CmdSync(CmdSync &&other);
    ~CmdSync();

    void create(VkDevice device);

    const VkSemaphore &getSemaphore() const { return _semaphore; }
    const VkFence     &getFence()     const { return _fence;     }

private:
    VkDevice    _device; //if device == VK_NULL_HANDLE, then it's a copy of a resource
    VkSemaphore _semaphore;
    VkFence     _fence;
    void clear();
};