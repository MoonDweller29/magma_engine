#include "magma/vk/textures/Texture.h"

#include "magma/vk/vulkan_common.h"
#include <vulkan/vulkan.hpp>

Texture::Texture(vk::Image img, vk::DeviceMemory mem, ImageView defaultImageView, TextureInfo* info)
    : _image(img), 
    _imageMemory(mem), 
    _defaultImageView(defaultImageView), 
    _info(info)
{}

[[deprecated]] Texture::Texture(VkImage c_img, VkDeviceMemory c_mem, ImageView defaultImageView, TextureInfo* info)
    : Texture(vk::Image(c_img), vk::DeviceMemory(c_mem), defaultImageView, info)
{}