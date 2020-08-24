#include <vulkan/vulkan.h>
#include "physicalDevice.h"
#include "buffer.h"
#include <vector>
#include <cstring>

class LogicalDevice
{
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkCommandPool graphicsCmdPool;

    void acquireQueues(QueueFamilyIndices indices);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    template<class T>
    Buffer createStagedBuffer(const std::vector<T> &data, VkBufferUsageFlags usageFlags);
public:


    LogicalDevice(const PhysicalDevice &physicalDevice);
    VkDevice handler() const { return device; }
    VkQueue getGraphicsQueue() const { return graphicsQueue; }
    VkQueue getPresentQueue() const { return presentQueue; }
    const VkCommandPool &getGraphicsCmdPool() const { return graphicsCmdPool; }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // buffer management
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    Buffer createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
    template<class T>
    Buffer createVertexBuffer(const std::vector<T> &data);
    template<class T>
    Buffer createIndexBuffer(const std::vector<T> &data);

    void deleteBuffer(Buffer &buffer);
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    ~LogicalDevice();
};

#include "logicalDevice.hpp"