#pragma once
#include <vulkan/vulkan.hpp>
#include "magma/vk/vulkan_common.h"

class Context {
public:
    Context();
    NON_COPYABLE(Context);
    ~Context();

    const vk::Instance &getInstance()   { return _instance; }
private:
    vk::Instance _instance;
};