/**
 * @file TextureManager.h
 * @brief Create and control textures
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <string>
#include <unordered_map>

#include "magma/vk/commandBuffer.h"
#include "magma/vk/textures/Texture.h"

class LogicalDevice;

class TextureManager {
public:
    TextureManager(LogicalDevice &device);
    ~TextureManager();

    bool textureExists(const std::string &name) const;
    Texture &getTexture(const std::string &name);

    Texture loadTexture(const std::string &texName, const std::string &path);

    Texture &createTexture2D(const std::string &name, VkFormat format, VkExtent2D extent,
        VkImageUsageFlags usage, VkImageAspectFlags aspectMask);
    Texture &createTexture2D(const std::string &name, VkFormat format, uint width, uint height,
        VkImageUsageFlags usage, VkImageAspectFlags aspectMask);

    void setLayout(Texture &texture, VkImageLayout newLayout);
    void copyBufToTex(Texture &texture, VkBuffer buffer);

    void deleteTexture(Texture &texture);
private:
    LogicalDevice &_device;
    CommandBufferArr _commandBuffers;
    std::unordered_map<std::string, Texture> _textures;
};