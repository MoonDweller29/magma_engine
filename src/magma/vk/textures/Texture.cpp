/**
 * @file Texture.cpp
 * @brief Class contain all information about texture
 * @version 0.1
 * @date 2021-02-12
 */
#include "magma/vk/textures/Texture.h"

#include "magma/vk/vulkan_common.h"

Texture::Texture()
    : _image(VK_NULL_HANDLE),
    _imageMemory(VK_NULL_HANDLE),
    _defaultImageView(VK_NULL_HANDLE),
    _info(NULL)
{}

Texture::Texture(VkImage img, VkDeviceMemory mem, ImageView defaultImageView, TextureInfo* info)
    : _image(img), 
    _imageMemory(mem), 
    _defaultImageView(defaultImageView), 
    _info(info)
{}
