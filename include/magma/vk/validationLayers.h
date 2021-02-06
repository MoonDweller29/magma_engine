#pragma once
#include <vulkan/vulkan.hpp>

#include <vector>

class ValidationLayers {
public:
    static const bool ENABLED;
    static const std::vector<const char*> validationLayers;

    static bool supported();
};


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