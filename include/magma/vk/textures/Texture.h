/**
 * @file Texture.h
 * @brief Class contain all information about texture
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <string>
#include <vulkan/vulkan.h>
#include <memory>

#include "magma/vk/textures/ImageView.h"

struct TextureInfo
{
    VkDevice                device;
    VkImageCreateInfo       imageInfo;
    VkImageViewCreateInfo   viewInfo;
    VkImageLayout           curLayout;
    std::string             name;
};

class Texture {
public:
    Texture();
    const VkImage           &getImage()     const { return _image;                      }
    const VkDeviceMemory    &getMemory()    const { return _imageMemory;                }
    const VkImageView       &getView()      const { return _defaultImageView.getView(); }
    const ImageView         &getImageView() const { return _defaultImageView;           }
    TextureInfo*             getInfo()      const { return _info;                       }

    friend class TextureManager;
private:
    Texture(VkImage img, VkDeviceMemory mem, ImageView defaultImageView, TextureInfo* info);

    VkImage _image;
    VkDeviceMemory _imageMemory;
    ImageView _defaultImageView;
    TextureInfo* _info;
};