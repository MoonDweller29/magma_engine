#include "magma/vk/validationLayers/DebugMessenger.h"

#include <iostream>
#include "magma/vk/vulkan_common.h"

DebugMessenger::Config DebugMessenger::_config;

void DebugMessenger::initConfig(const DebugMessenger::Config& config) {
    _config = config;
}

//VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
//VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
//VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error, but very likely a bug in your application
//VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause crashes
Log::Level DebugMessenger::msgSeverityToLogLevel(vk::DebugUtilsMessageSeverityFlagBitsEXT msgSeverity) {
    switch (msgSeverity) {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            return Log::Level::DEBUG;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            return Log::Level::INFO;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            return Log::Level::WARNING;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            return Log::Level::ERROR;
        default:
            return Log::Level::CRITICAL;
    }
}

VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessenger::debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT messageType,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData
) {
    auto msgSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT(messageSeverity);
    if (msgSeverity >= _config.minSeverity) {
        Log::message(
                DebugMessenger::msgSeverityToLogLevel(msgSeverity),
                "[VALIDATION LAYER] ", pCallbackData->pMessage);
    }

    return VK_FALSE;
}


vk::DebugUtilsMessengerCreateInfoEXT DebugMessenger::getCreateInfo() {
    auto msgSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose  |
                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo     |
                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning  |
                       vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

    auto msgType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral    |
                   vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                   vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

    vk::DebugUtilsMessengerCreateInfoEXT createInfo({}, msgSeverity, msgType, debugCallback);

    return createInfo;
}

DebugMessenger::DebugMessenger(const vk::Instance &instance) : _instance(instance) {
    if (!ValidationLayers::ENABLED)
        return;

    auto createInfo = getCreateInfo();
    vk::Result result;
    std::tie(result, _debugMessenger) = _instance.createDebugUtilsMessengerEXT(createInfo);
    VK_HPP_CHECK_ERR(result, "failed to set up debug messenger!");
}

DebugMessenger::~DebugMessenger() {
    if (ValidationLayers::ENABLED) {
        _instance.destroyDebugUtilsMessengerEXT(_debugMessenger);
    }
}