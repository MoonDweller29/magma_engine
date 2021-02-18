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
    CustomImageView(Texture &texture, VkImageAspectFlags aspectMask);
    CustomImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
    CustomImageView(CustomImageView &&other);
    ~CustomImageView();

    CustomImageView(const CustomImageView &) = delete;
private:
    VkDevice _device;
};