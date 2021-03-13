#include "magma/vk/vulkan_common.h"

bool isSuccess(VkResult result) {
    return result == VK_SUCCESS;
}

bool isSuccess(vk::Result result) {
    return result == vk::Result::eSuccess;
}
