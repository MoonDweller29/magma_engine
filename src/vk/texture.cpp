#include "texture.h"

Texture::Texture():
    image(VK_NULL_HANDLE), imageMemory(VK_NULL_HANDLE)
{}

Texture::Texture(VkImage img, VkDeviceMemory mem, VkImageTiling tiling):
    image(img), imageMemory(mem), tiling(tiling)
{}
