#pragma once
#include "magma/vk/validationLayers/ValidationLayers.h"
#include "magma/app/log.hpp"

class DebugMessenger {
public:
    static Log::Level msgSeverityToLogLevel(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity);
    static void fillCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    DebugMessenger(const VkInstance &vk_instance);
    const VkDebugUtilsMessengerEXT &handler() { return debugMessenger; }
    ~DebugMessenger();
private:
    VkDebugUtilsMessengerEXT debugMessenger;
    VkInstance instance;
};