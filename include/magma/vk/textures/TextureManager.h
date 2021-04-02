/**
 * @file TextureManager.h
 * @brief Tool for texture creation and control
 * @version 0.1
 * @date 2021-02-12
 */
#pragma once
#include <string>
#include <unordered_map>

#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/textures/Texture.h"
#include "magma/vk/vulkan_common.h"

class LogicalDevice;

class TextureManager {
public:
    TextureManager(LogicalDevice &device);
    NON_COPYABLE(TextureManager);
    ~TextureManager();

    bool textureExists(const std::string &name) const;
    Texture& getTexture(const std::string &name);

    Texture& loadTexture(const std::string &texName, const std::string &path);

    Texture& createTexture2D(const std::string &name, vk::Format format, vk::Extent2D extent,
        vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectMask);

    void setLayout(Texture &texture, vk::ImageLayout newLayout);
    void copyBufToTex(Texture &texture, vk::Buffer buffer);

    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates,
                                    vk::ImageTiling tiling, vk::FormatFeatureFlags features);
    vk::Format findDepthFormat();

    void deleteTexture(Texture &texture);

private:
    LogicalDevice &_device;
    CommandBuffer _commandBuffer;
    std::unordered_map<std::string, Texture> _textures;
};