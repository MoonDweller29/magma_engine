#include "logicalDevice.h"

#include "vulkan_common.h"
#include "validationLayers.h"
#include <iostream>
#include <set>

LogicalDeviceHolder::LogicalDeviceHolder(const PhysicalDevice &physicalDevice)
{
    QueueFamilyIndices indices = physicalDevice.getQueueFamilyInds();

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

    VkPhysicalDeviceFeatures deviceFeatures{}; //will be used later

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(PhysicalDevice::extensions.size());
    createInfo.ppEnabledExtensionNames = PhysicalDevice::extensions.data();

    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    VkResult result = vkCreateDevice(physicalDevice.device(), &createInfo, nullptr, &device);
    vk_check_err(result, "failed to create logical device!");

    std::cout << "Logical Device is created\n";
    acquireQueues(indices);
}

void LogicalDeviceHolder::acquireQueues(QueueFamilyIndices indices)
{
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);

    std::cout << "graphicsFamily ind : " << indices.graphicsFamily.value() << std::endl;
    std::cout << "presentFamily ind : " << indices.presentFamily.value() << std::endl;

    std::cout << "graphicsFamily : " << graphicsQueue << std::endl;
    std::cout << "presentFamily  : " << presentQueue << std::endl;
}


LogicalDeviceHolder::~LogicalDeviceHolder()
{
    vkDestroyDevice(device, nullptr);
}