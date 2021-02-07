#pragma once
#include "magma/vk/validationLayers/ValidationLayers.h"
#include "magma/app/log.hpp"

class DebugMessenger {
public:
    static Log::Level msgSeverityToLogLevel(vk::DebugUtilsMessageSeverityFlagBitsEXT msgSeverity);
    static vk::DebugUtilsMessengerCreateInfoEXT getCreateInfo();

    DebugMessenger(const vk::Instance &instance);
    ~DebugMessenger();
private:
    vk::DebugUtilsMessengerEXT debugMessenger;
    vk::Instance _instance;
};