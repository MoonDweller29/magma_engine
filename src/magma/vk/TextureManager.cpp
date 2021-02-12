/**
 * @file TextureManager.cpp
 * @brief Create and control textures
 * @version 0.1
 * @date 2021-02-12
 */
#include "magma/vk/TextureManager.h"

TextureManager::TextureManager(LogicalDevice &device) 
    : _device(device) 
{}

TextureManager::~TextureManager() {

}

Texture TextureManager::getTexture(const std::string &name) {
    return _textures[name];
}

Texture TextureManager::createTexture(std::string &name, VkFormat format, VkExtent3D extend, VkImageUsageFlags usage) {
    
}

Texture TextureManager::createTexture2D(uint32_t width, uint32_t height,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, 
        VkMemoryPropertyFlags properties, VkImageAspectFlags aspectFlags) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags = 0; // Optional

    VkImage textureImage;
    VkResult result = vkCreateImage(device, &imageInfo, nullptr, &textureImage);
    VK_CHECK_ERR(result, "failed to create image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device, textureImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

    VkDeviceMemory textureImageMemory;
    result = vkAllocateMemory(device, &allocInfo, nullptr, &textureImageMemory);
    VK_CHECK_ERR(result, "failed to allocate image memory!");

    vkBindImageMemory(device, textureImage, textureImageMemory, 0);

    return Texture(textureImage, textureImageMemory, device, imageInfo, aspectFlags);
}