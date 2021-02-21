/**
 * @file CustomImageView.h
 * @brief RAII Class containing image view
 * @version 0.1
 * @date 2021-02-13
 */
#pragma once
#include <vulkan/vulkan.h>

#include "magma/vk/textures/ImageView.h"
#include "magma/vk/textures/Texture.h"

class CustomImageView final : public ImageView {
public:
    CustomImageView(Texture &texture, vk::ImageAspectFlags aspectMask);
    CustomImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectMask);
    CustomImageView(CustomImageView &&other);
    ~CustomImageView();

    [[deprecated]] CustomImageView(Texture &texture, VkImageAspectFlags c_aspectMask);
    [[deprecated]] CustomImageView(VkDevice c_device, VkImage c_image, VkFormat c_format, VkImageAspectFlags c_aspectMask);

    CustomImageView(const CustomImageView &) = delete;
private:
    vk::Device _device;
};