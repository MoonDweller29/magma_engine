#pragma once
#include "magma/vk/validationLayers/ValidationLayers.h"

class DebugMessenger
{
    VkDebugUtilsMessengerEXT debugMessenger;
    VkInstance instance;
public:
    DebugMessenger(const VkInstance &vk_instance);

    static void fillCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    const VkDebugUtilsMessengerEXT &handler() { return debugMessenger; }

    ~DebugMessenger();
};