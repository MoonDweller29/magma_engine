#pragma once
#include <vulkan/vulkan.h>

class CmdSync
{
    VkDevice device; //if device == VK_NULL_HANDLE, then it's a copy of a resource
    void clear();
public:
    VkSemaphore semaphore;
    VkFence fence;
    CmdSync();
    CmdSync(VkDevice device);
    void create(VkDevice device);
    CmdSync(CmdSync &other);
    ~CmdSync();
};