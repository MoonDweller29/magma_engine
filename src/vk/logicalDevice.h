#include <vulkan/vulkan.h>
#include "physicalDevice.h"


class LogicalDeviceHolder
{
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    void acquireQueues(QueueFamilyIndices indices);

public:
    LogicalDeviceHolder(const PhysicalDevice &physicalDevice);
    VkDevice handler() const { return device; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    ~LogicalDeviceHolder();
};