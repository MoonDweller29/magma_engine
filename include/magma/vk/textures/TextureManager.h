/**
 * @file TextureManager.h
 * @brief Tool for texture creation and control
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
    Texture& getTexture(const std::string &name);

    Texture& loadTexture(const std::string &texName, const std::string &path);

    Texture& createTexture2D(const std::string &name, vk::Format format, vk::Extent2D extent,
        vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectMask);
    [[deprecated]] Texture& createTexture2D(const std::string &name, VkFormat c_format, VkExtent2D c_extent,
        VkImageUsageFlags c_usage, VkImageAspectFlags c_aspectMask);

    void setLayout(Texture &texture, vk::ImageLayout newLayout);
    void copyBufToTex(Texture &texture, vk::Buffer buffer);

    [[deprecated]] void setLayout(Texture &texture, VkImageLayout c_newLayout);
    [[deprecated]] void copyBufToTex(Texture &texture, VkBuffer c_buffer);

    void deleteTexture(Texture &texture);
private:
    LogicalDevice &_device;
    CommandBufferArr _commandBuffers;
    std::unordered_map<std::string, Texture> _textures;
};