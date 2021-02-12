/**
 * @file Texture.cpp
 * @brief Class contain all information about texture
 * @version 0.1
 * @date 2021-02-12
 */
#include "magma/vk/Texture.h"
#include "magma/vk/vulkan_common.h"

Texture::Texture():
    image(VK_NULL_HANDLE), imageMemory(VK_NULL_HANDLE), imageView(VK_NULL_HANDLE)
{}

Texture::Texture(VkImage img, VkDeviceMemory mem,
                 VkDevice device, VkImageCreateInfo imageCreateInfo, VkImageAspectFlags aspectFlags):
    image(img), imageMemory(mem)
{
    info.device = device;
    info.imageInfo = imageCreateInfo;
    info.defaultAspectFlags = aspectFlags;
    info.viewInfo = getDefaultViewInfo();

    VkResult result = vkCreateImageView(device, &info.viewInfo, nullptr, &imageView);
    VK_CHECK_ERR(result, "failed to create texture image view!");
}

VkImageViewCreateInfo Texture::getDefaultViewInfo()
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;

    switch (info.imageInfo.imageType)
    {
        case VK_IMAGE_TYPE_2D:
            viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; break;
        default:
            throw std::runtime_error("wrong imageType");
    }

    viewInfo.format = info.imageInfo.format;
    viewInfo.subresourceRange.aspectMask = info.defaultAspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = info.imageInfo.mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = info.imageInfo.arrayLayers;

    return viewInfo;
}

void Texture::deleteView()
{
    vkDestroyImageView(info.device, imageView, nullptr);
    imageView = VK_NULL_HANDLE;
}

void Texture::deleteImage()
{
    vkDestroyImage(info.device, image, nullptr);
    image = VK_NULL_HANDLE;
}

void Texture::freeMem()
{
    vkFreeMemory(info.device, imageMemory, nullptr);
    imageMemory = VK_NULL_HANDLE;
}

void Texture::setView(VkImageViewCreateInfo viewInfo)
{
    deleteView();
    info.viewInfo = viewInfo;

    VkResult result = vkCreateImageView(info.device, &info.viewInfo, nullptr, &imageView);
    VK_CHECK_ERR(result, "failed to create texture image view!");
}
