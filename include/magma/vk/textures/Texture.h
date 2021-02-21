/**
 * @file Texture.h
 * @brief Class containig all information about texture
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <string>
#include <vulkan/vulkan.hpp>
#include <memory>

#include "magma/vk/textures/ImageView.h"

struct TextureInfo
{
    vk::Device              device;
    vk::ImageCreateInfo     imageInfo;
    vk::ImageViewCreateInfo viewInfo;
    vk::ImageLayout         curLayout;
    std::string             name;
};

class Texture {
friend class TextureManager;

public:
    Texture() = default;

    const vk::Image         getImage()      const { return _image;                      }
    const vk::DeviceMemory  getMemory()     const { return _imageMemory;                }
    const vk::ImageView     getView()       const { return _defaultImageView.getView(); }
    const ImageView         &getImageView() const { return _defaultImageView;           }
    TextureInfo*            getInfo()       const { return _info;                       }

    [[depricated]]  const VkImage           &c_getImage()     const { return (VkImage)_image;                           }
    [[depricated]]  const VkDeviceMemory    &c_getMemory()    const { return (VkDeviceMemory)_imageMemory;              }
    [[depricated]]  const VkImageView       &c_getView()      const { return (VkImageView)_defaultImageView.getView();  }

private:
    Texture(vk::Image img, vk::DeviceMemory mem, ImageView defaultImageView, TextureInfo* info);
    [[depricated]] Texture(VkImage img, VkDeviceMemory mem, ImageView defaultImageView, TextureInfo* info);

    vk::Image           _image;
    vk::DeviceMemory    _imageMemory;
    ImageView           _defaultImageView;
    TextureInfo*        _info;
};