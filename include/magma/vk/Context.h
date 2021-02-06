#pragma once
#include <vulkan/vulkan.hpp>

class Context {
public:
    const VkInstance c_instance(){ return static_cast<VkInstance>(_instance); }

    Context();
    ~Context();
private:
    vk::Instance _instance;
};