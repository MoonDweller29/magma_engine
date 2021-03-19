#pragma once
#include <vulkan/vulkan.hpp>

class CmdSync {
public:
    CmdSync(vk::Device device, bool makeFenceSignaled = true);
    CmdSync(CmdSync &other);
    CmdSync(CmdSync &&other);
    ~CmdSync();

    const vk::Semaphore &getSemaphore() const { return _semaphore; }
    const vk::Fence     &getFence()     const { return _fence;     }

    void waitForFence() const;
    void resetFence();
    bool isFenceSignaled() const;

private:
    bool          _isResourceOwner;
    vk::Device    _device;
    vk::Semaphore _semaphore;
    vk::Fence     _fence;
};