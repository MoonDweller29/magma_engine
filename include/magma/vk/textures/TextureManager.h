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

    bool textureExist(const std::string &name) const;
    Texture &getTexture(const std::string &name);

    Texture &createTexture2D(const std::string &name, VkFormat format, VkExtent3D extent,
        VkImageUsageFlags usage, VkImageAspectFlags aspectMask);

    void setLayout(Texture &texture, VkImageLayout newLayout);
    void copyFromBuffer(Texture &texture, VkBuffer buffer);

    void deleteTexture(Texture &texture);
private:
    LogicalDevice &_device;
    CommandBufferArr _commandBuffers;
    std::unordered_map<std::string, Texture> _textures;
};