#include "magma/vk/LogicalDevice.h"

#include <iostream>
#include <set>

#include "magma/vk/vulkan_common.h"
#include "magma/vk/validationLayers/ValidationLayers.h"
#include "magma/vk/commands/CommandPool.h"


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
    VK_CHECK_ERR(result, "failed to create logical device!");
    LOG_INFO("Logical Device is created");

    acquireQueues(indices);

    _textureManager = std::make_unique<TextureManager>(*this);
    _bufferManager = std::make_unique<BufferManager>(*this);
}

void LogicalDevice::acquireQueues(QueueFamilyIndices indices) {
    _graphicsQueue.queue = _device.getQueue(indices.graphicsFamily.value(), 0);
    _graphicsQueue.queueFamily = indices.graphicsFamily.value();
    _graphicsQueue.cmdPool = CommandPool::createPool(_device, indices.graphicsFamily.value());

    _presentQueue.queue  = _device.getQueue(indices.presentFamily.value(), 0);
    _presentQueue.queueFamily = indices.presentFamily.value();


    std::cout << "graphicsFamily ind : " << indices.graphicsFamily.value() << std::endl;
    std::cout << "presentFamily ind : " << indices.presentFamily.value() << std::endl;
}

vk::DeviceMemory LogicalDevice::memAlloc(vk::MemoryRequirements memRequirements, vk::MemoryPropertyFlags properties) {
    vk::MemoryAllocateInfo allocInfo(
            memRequirements.size,
            _physDevice.findMemoryTypeInd(memRequirements.memoryTypeBits, properties)
    );

    auto [result, deviceMemory] = _device.allocateMemory(allocInfo);
    VK_CHECK_ERR(result, "failed to allocate memory!");
    return deviceMemory;
}

void LogicalDevice::waitIdle() {
    vk::Result result = _device.waitIdle();
}


LogicalDevice::~LogicalDevice() {
    _textureManager.reset();
    _bufferManager.reset();
    _device.destroyCommandPool(_graphicsQueue.cmdPool);
    _device.destroy();
}
