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
#include "magma/vk/vulkan_common.h"

class CustomImageView final : public ImageView {
public:
    CustomImageView(Texture &texture, vk::ImageAspectFlags aspectMask);
    CustomImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectMask);
    CustomImageView(CustomImageView &&other);
    NON_COPYABLE(CustomImageView);
    ~CustomImageView();

private:
    vk::Device _device;
};