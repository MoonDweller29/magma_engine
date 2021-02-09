#pragma once
#include "magma/vk/validationLayers/ValidationLayers.h"
#include "magma/app/log.hpp"
#include "magma/app/config/JSON.h"

class DebugMessenger {
public:
    struct Config {
        vk::DebugUtilsMessageSeverityFlagBitsEXT minSeverity =
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose;
    private:
        JSON_MAPPINGS(
            { minSeverity, "minimal_severity"}
        )
    };
    static void initConfig(const Config& config);

    static Log::Level msgSeverityToLogLevel(vk::DebugUtilsMessageSeverityFlagBitsEXT msgSeverity);
    static vk::DebugUtilsMessengerCreateInfoEXT getCreateInfo();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
    );

    DebugMessenger(const vk::Instance &instance);
    ~DebugMessenger();
private:
    vk::DebugUtilsMessengerEXT _debugMessenger;
    vk::Instance _instance;

    static Config _config;
};

JSON_ENUM_MAPPING(vk::DebugUtilsMessageSeverityFlagBitsEXT,
    { vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,  "eDEBUG"    },
    { vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo,     "eINFO"     },
    { vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning,  "eWARNING"  },
    { vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,    "eERROR"    }
)

