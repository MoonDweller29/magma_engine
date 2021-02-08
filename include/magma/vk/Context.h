#pragma once
#include <vulkan/vulkan.hpp>

class Context {
public:
    const vk::Instance &instance()   { return _instance; }
    const VkInstance   &c_instance() { return _c_instance; } //only for Vulkan C API

    Context();
    ~Context();
private:
    vk::Instance _instance;
    VkInstance _c_instance;
};