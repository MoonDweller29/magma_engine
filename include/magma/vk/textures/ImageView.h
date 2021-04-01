/**
 * @file ImageView.h
 * @brief Class containing image view
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <vulkan/vulkan.hpp>

class ImageView {
public:
    ImageView() = default;
    ImageView(vk::ImageView imageView);

    const vk::ImageView getView() const { return _imageView; }
protected:
    vk::ImageView _imageView;
};
