/**
 * @file TextureManager.h
 * @brief Create and control textures
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <string>
#include <unordered_map>

#include "magma/vk/Texture.h"
#include "magma/vk/logicalDevice.h"

class TextureManager {
public:
    TextureManager(LogicalDevice &device);
    ~TextureManager();

    Texture getTexture(const std::string &name);

    Texture createTexture(std::string &name, VkFormat format, VkExtent3D extend, VkImageUsageFlags usage);

    Texture createTexture2D(uint32_t width, uint32_t height,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags);

    void deleteTexture(Texture &texture);
private:
    LogicalDevice &_device;
    std::unordered_map<std::string, Texture> _textures;
};