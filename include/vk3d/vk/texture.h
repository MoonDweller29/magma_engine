#pragma once
#include <vulkan/vulkan.h>

struct TextureInfo
{
    VkDevice device;
    VkImageCreateInfo imageInfo;
    VkImageViewCreateInfo viewInfo;
    VkImageAspectFlags defaultAspectFlags;
};

class Texture
{
    VkImage image;
    VkDeviceMemory imageMemory;
    VkImageView imageView;

    TextureInfo info;

    void deleteView();
    void deleteImage();
    void freeMem();
public:
    Texture();
    Texture(
            VkImage img, VkDeviceMemory mem,
            VkDevice device, VkImageCreateInfo imageCreateInfo, VkImageAspectFlags aspectFlags);

    const VkImage         &img()       const { return image; }
    const VkDeviceMemory  &mem()       const { return imageMemory; }
    const VkImageView     &view()      const { return imageView; }
    const TextureInfo     &getInfo()   const { return info; }


    VkImageViewCreateInfo getDefaultViewInfo();
    void setView(VkImageViewCreateInfo viewInfo);

    friend class LogicalDevice;
};