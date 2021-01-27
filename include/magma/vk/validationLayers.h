#pragma once
#include <vulkan/vulkan.h>
#include <vector>

extern const bool enableValidationLayers;
extern const std::vector<const char*> validationLayers;

bool check_validation_layer_support();

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