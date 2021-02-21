/**
 * @file ImageView.h
 * @brief Class containing image view
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_core.h>

class ImageView {
public:
    ImageView() = default;
    ImageView(vk::ImageView imageView);

    const vk::ImageView getView() const { return _imageView; }
    [[depricated]] const VkImageView c_getView() const { return (VkImageView)_imageView; }
protected:
    vk::ImageView _imageView;
};
