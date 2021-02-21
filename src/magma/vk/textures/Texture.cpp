#include "magma/vk/textures/Texture.h"

#include "magma/vk/vulkan_common.h"
#include <vulkan/vulkan.hpp>

Texture::Texture(vk::Image img, vk::DeviceMemory mem, ImageView defaultImageView, TextureInfo* info)
    : _image(img), 
    _imageMemory(mem), 
    _defaultImageView(defaultImageView), 
    _info(info)
{}
