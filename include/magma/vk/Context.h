#pragma once
#include <vulkan/vulkan.hpp>

class Context {
public:
    Context();
    ~Context();

    const vk::Instance &instance()   { return _instance; }
    const VkInstance   &c_instance() { return _c_instance; } //only for Vulkan C API
private:
    vk::Instance _instance;
    VkInstance _c_instance;
};