/**
 * @file TextureManager.cpp
 * @brief Create and control textures
 * @version 0.1
 * @date 2021-02-12
 */
#include "magma/vk/textures/TextureManager.h"

#include "magma/vk/Texture.h"
#include "magma/vk/vulkan_common.h"

TextureManager::TextureManager(LogicalDevice &device) 
    : _device(device) 
{}

TextureManager::~TextureManager() {}

Texture TextureManager::createTexture2D(std::string &name, VkFormat format, VkExtent3D extent,
        VkImageUsageFlags usage, VkImageAspectFlags aspectMask) {
    
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.format = format;
    imageInfo.extent = extent;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.flags = 0;

    VkImage textureImage;
    VkResult result = vkCreateImage(_device.handler(), &imageInfo, nullptr, &textureImage);
    VK_CHECK_ERR(result, "TextureManager::failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(_device.handler(), textureImage, &memRequirements);
    VkDeviceMemory textureMemory = _device.createDeviceMemory(memRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vkBindImageMemory(_device.handler(), textureImage, textureMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = textureImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView textureView;
    result = vkCreateImageView(_device.handler(), &viewInfo, nullptr, &textureView);
    VK_CHECK_ERR(result, "TextureManager::failed to create image view!");

    TextureInfo* textureInfo = new TextureInfo;
    textureInfo->imageInfo = imageInfo;
    textureInfo->viewInfo = viewInfo;
    textureInfo->curLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    _textures[name] = Texture(textureImage, textureMemory, ImageView(textureView), textureInfo);
}

void TextureManager::deleteTexture(Texture &texture) {
    delete texture.getInfo();
    vkDestroyImageView(_device.handler(), texture.getView().getImageView(), nullptr);
    vkDestroyImage(_device.handler(), texture.getImage(), nullptr);
    vkFreeMemory(_device.handler(), texture.getMemory(), nullptr);
}