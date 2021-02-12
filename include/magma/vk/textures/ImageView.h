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
    ImageView(VkImageView imageView);

    const VkImageView &getImageView() { return _imageView; }
private:
    VkImageView _imageView;
};

class CustomImageView : ImageView {
public:
    CustomImageView(VkDevice device);
    ~CustomImageView();
private:
    VkDevice _device;
};
