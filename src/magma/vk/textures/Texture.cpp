#include "magma/vk/textures/Texture.h"

#include "magma/vk/vulkan_common.h"

Texture::Texture(vk::Image img, vk::DeviceMemory mem, ImageView defaultImageView, TextureInfo* info)
    : _image(img), 
    _imageMemory(mem), 
    _defaultImageView(defaultImageView), 
    _info(info)
{}

[[depricated]] Texture::Texture(VkImage c_img, VkDeviceMemory c_mem, ImageView defaultImageView, TextureInfo* info)
    : _image(c_img), 
    _imageMemory(c_mem), 
    _defaultImageView(defaultImageView), 
    _info(info)
{}