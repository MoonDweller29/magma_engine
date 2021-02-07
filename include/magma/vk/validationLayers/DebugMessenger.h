#pragma once
#include "magma/vk/validationLayers/ValidationLayers.h"
#include "magma/app/log.hpp"

class DebugMessenger {
public:
    static Log::Level msgSeverityToLogLevel(VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity);
    static VkDebugUtilsMessengerCreateInfoEXT getCreateInfo();

    DebugMessenger(const VkInstance &vk_instance);
    ~DebugMessenger();
private:
    VkDebugUtilsMessengerEXT debugMessenger;
    VkInstance instance;
};