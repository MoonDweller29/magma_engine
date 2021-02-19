#include "magma/vk/logicalDevice.h"

#include "magma/vk/commandPool.h"
#include "magma/vk/vulkan_common.h"
#include "magma/vk/validationLayers/ValidationLayers.h"
#include "magma/vk/commandBuffer.h"
#include <iostream>
#include <memory>
#include <set>

LogicalDevice::LogicalDevice(
        const PhysicalDevice &physicalDevice, const std::vector<const char*> &deviceExtensions
) : _physDevice(physicalDevice)
{
    QueueFamilyIndices indices = _physDevice.getQueueFamilyInds();

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
    };
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (ValidationLayers::ENABLED)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers::validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(_physDevice.c_device(), &createInfo, nullptr, &device);
    VK_CHECK_ERR(result, "failed to create logical device!");

    std::cout << "Logical Device is created\n";
    acquireQueues(indices);
    graphicsCmdPool = CommandPool::createPool(device, indices.graphicsFamily.value());

    _textureManager = std::make_unique<TextureManager>(*this);
    _bufferManager = std::make_unique<BufferManager>(*this);
}

void LogicalDevice::acquireQueues(QueueFamilyIndices indices)
{
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    std::cout << "graphicsFamily ind : " << indices.graphicsFamily.value() << std::endl;
    std::cout << "presentFamily ind : " << indices.presentFamily.value() << std::endl;

    std::cout << "graphicsFamily : " << graphicsQueue << std::endl;
    std::cout << "presentFamily  : " << presentQueue << std::endl;
}


static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

VkDeviceMemory LogicalDevice::createDeviceMemory(VkMemoryRequirements memRequirements, VkMemoryPropertyFlags properties) {
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(_physDevice.c_device(), memRequirements.memoryTypeBits, properties);

    VkDeviceMemory deviceMemory;
    VkResult result = vkAllocateMemory(device, &allocInfo, nullptr, &deviceMemory);
    VK_CHECK_ERR(result, "failed to allocate memory!");
    return deviceMemory;
}

LogicalDevice::~LogicalDevice()
{
    _textureManager.reset();
    vkDestroyCommandPool(device, graphicsCmdPool, nullptr);
    vkDestroyDevice(device, nullptr);
}
