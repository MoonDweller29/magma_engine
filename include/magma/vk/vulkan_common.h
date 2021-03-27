#pragma once
#include <vulkan/vulkan.hpp>
#include <stdexcept>

#include "magma/app/log.hpp"

bool isSuccess(VkResult result);

bool isSuccess(vk::Result result);

#define VK_CHECK_ERR(result, message)               \
    if (!isSuccess(result)) {                       \
        LOG_AND_THROW std::runtime_error(message);  \
    }

vk::Extent2D toExtent2D(const vk::Extent3D &extent3D);