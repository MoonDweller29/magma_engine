#pragma once
#include <vulkan/vulkan.h>

class Texture
{
    VkImage image;
    VkDeviceMemory imageMemory;

    //Texture info
    VkImageTiling tiling;
public:
    Texture();
    Texture(VkImage img, VkDeviceMemory mem, VkImageTiling tiling);
    const VkImage &img() const { return image; }
    const VkDeviceMemory &mem() const { return imageMemory; }
};