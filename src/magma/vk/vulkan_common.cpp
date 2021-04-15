#include "magma/vk/vulkan_common.h"

bool isSuccess(VkResult result) {
    return result == VK_SUCCESS;
}

bool isSuccess(vk::Result result) {
    return result == vk::Result::eSuccess;
}

vk::Extent2D toExtent2D(const vk::Extent3D &extent3D) {
    return vk::Extent2D(extent3D.width, extent3D.height);
}

std::string toString(const vk::Extent2D &extent) {
    return std::string("{ ") + std::to_string(extent.width) +
        " " + std::to_string(extent.height) + " }";
}