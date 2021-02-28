#pragma once
#include <vulkan/vulkan.hpp>
#include <stdexcept>

#include "magma/app/log.hpp"

#define VK_CHECK_ERR(result, message)      \
{                                          \
    if (result != VK_SUCCESS) {            \
       LOG_ERROR(message);                 \
       throw std::runtime_error(message);  \
    }                                      \
}

#define VK_HPP_CHECK_ERR(result, message)  \
{                                          \
    if (result != vk::Result::eSuccess) {  \
       LOG_ERROR(message);                 \
       throw std::runtime_error(message);  \
    }                                      \
}
