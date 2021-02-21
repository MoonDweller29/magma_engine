#include "magma/vk/logicalDevice.h"

#include "magma/vk/commandPool.h"
#include "magma/vk/vulkan_common.h"
#include "magma/vk/validationLayers/ValidationLayers.h"
#include "magma/vk/commandBuffer.h"
#include <iostream>
#include <set>

LogicalDevice::LogicalDevice(
        const PhysicalDevice              &physicalDevice,
        const vk::PhysicalDeviceFeatures  &physicalDeviceFeatures,
        const std::vector<const char*>    &deviceExtensions
) : _physDevice(physicalDevice)
{
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    QueueFamilyIndices indices = _physDevice.getQueueFamilyInds();
    std::set<uint32_t> uniqueQueueFamilies = {
            indices.graphicsFamily.value(),
            indices.presentFamily.value()
    };

    std::vector<float> queuePriorities = { 1.0f };
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        vk::DeviceQueueCreateInfo queueCreateInfo({}, queueFamily, queuePriorities);
        queueCreateInfos.push_back(queueCreateInfo);
    }

    vk::DeviceCreateInfo createInfo;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &physicalDeviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (ValidationLayers::ENABLED) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(ValidationLayers::validationLayers.size());
        createInfo.ppEnabledLayerNames = ValidationLayers::validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    vk::Result result;
    std::tie(result, _device) = _physDevice.device().createDevice(createInfo);
    VK_HPP_CHECK_ERR(result, "failed to create logical device!");
    LOG_INFO("Logical Device is created");

    acquireQueues(indices);
    graphicsCmdPool = CommandPool::createPool(_device, indices.graphicsFamily.value());

    _textureManager = std::make_unique<TextureManager>(*this);
    _bufferManager = std::make_unique<BufferManager>(*this);
}

void LogicalDevice::acquireQueues(QueueFamilyIndices indices) {
    graphicsQueue = _device.getQueue(indices.graphicsFamily.value(), 0);
    presentQueue  = _device.getQueue(indices.presentFamily.value(), 0);

    std::cout << "graphicsFamily ind : " << indices.graphicsFamily.value() << std::endl;
    std::cout << "presentFamily ind : " << indices.presentFamily.value() << std::endl;

    std::cout << "graphicsFamily : " << graphicsQueue << std::endl;
    std::cout << "presentFamily  : " << presentQueue << std::endl;
}


static uint32_t findMemoryType(vk::PhysicalDevice physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    uint32_t memTypeInd = -1;
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            memTypeInd = i;
            break;
        }
    }

    if (memTypeInd == -1) {
        LOG_AND_THROW std::runtime_error("failed to find suitable memory type!");
    }

    return memTypeInd;
}

vk::DeviceMemory LogicalDevice::memAlloc(vk::MemoryRequirements memRequirements, vk::MemoryPropertyFlags properties) {
    vk::MemoryAllocateInfo allocInfo(
            memRequirements.size,
            findMemoryType(_physDevice.device(), memRequirements.memoryTypeBits, properties)
    );

    auto [result, deviceMemory] = _device.allocateMemory(allocInfo);
    VK_HPP_CHECK_ERR(result, "failed to allocate memory!");
    return deviceMemory;
}

LogicalDevice::~LogicalDevice() {
    _textureManager.reset();
    _bufferManager.reset();
    _device.destroyCommandPool(graphicsCmdPool);
    _device.destroy();
}
