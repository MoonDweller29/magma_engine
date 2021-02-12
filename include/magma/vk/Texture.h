/**
 * @file Texture.h
 * @brief Class contain all information about texture
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <vulkan/vulkan.h>
#include <memory>

struct TextureInfo
{
    VkDevice device;
    VkImageCreateInfo imageInfo;
    VkImageViewCreateInfo viewInfo;
    VkImageAspectFlags defaultAspectFlags;
};

class Texture {
public:
    Texture();
    Texture(
            VkImage img, VkDeviceMemory mem,
            VkDevice device, VkImageCreateInfo imageCreateInfo, VkImageAspectFlags aspectFlags);

    const VkImage         &img()       const { return *_image; }
    const VkDeviceMemory  &mem()       const { return *_imageMemory; }
    const VkImageView     &view()      const { return *_imageView; }
    const TextureInfo     &getInfo()   const { return *_info; }


    VkImageViewCreateInfo getDefaultViewInfo();
    void setView(VkImageViewCreateInfo viewInfo);

    friend class LogicalDevice;
private:
    VkImage _image;
    VkDeviceMemory _imageMemory;
    std::shared_ptr<VkImageView> _imageView;
    TextureInfo* _info;

    void deleteView();
    void deleteImage();
    void freeMem();
};