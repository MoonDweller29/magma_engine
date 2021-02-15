/**
 * @file Texture.h
 * @brief Class contain all information about texture
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <vulkan/vulkan.h>
#include <memory>

#include "magma/vk/textures/ImageView.h"

struct TextureInfo
{
    VkImageCreateInfo       imageInfo;
    VkImageViewCreateInfo   viewInfo;
    VkImageLayout           curLayout;
};

class Texture {
public:
    Texture();
    Texture(VkImage img, VkDeviceMemory mem, ImageView defaultImageView, TextureInfo* info);

    const VkImage           &getImage()     const { return _image;              }
    const VkDeviceMemory    &getMemory()    const { return _imageMemory;        }
    const ImageView         &getView()      const { return _defaultImageView;   }
    TextureInfo*             getInfo()      const { return _info;               }

private:
    VkDevice _device;

    VkImage _image;
    VkDeviceMemory _imageMemory;
    ImageView _defaultImageView;
    TextureInfo* _info;
};