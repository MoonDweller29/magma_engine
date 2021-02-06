#pragma once
#include <vulkan/vulkan.hpp>

class Context {
public:
    const VkInstance &c_instance(){ return _c_instance; }

    Context();
    ~Context();
private:
    vk::Instance _instance;
    VkInstance _c_instance;
};