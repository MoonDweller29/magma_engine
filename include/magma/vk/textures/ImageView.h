/**
 * @file ImageView.h
 * @brief Class contain image view
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <vulkan/vulkan.h>

class ImageView {
public:
    ImageView();
    ImageView(VkImageView imageView);

    const VkImageView &getImageView() const { return _imageView; }
protected:
    VkImageView _imageView;
};
