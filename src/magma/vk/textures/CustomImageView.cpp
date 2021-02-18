#include "magma/vk/textures/CustomImageView.h"

#include "magma/vk/vulkan_common.h"
#include <vulkan/vulkan_core.h>

CustomImageView::CustomImageView(Texture &texture, VkImageAspectFlags aspectMask) 
        : _device(texture.getInfo()->device) {
    VkImageCreateInfo info = texture.getInfo()->imageInfo;
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = texture.getImage();
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = info.format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(_device, &viewInfo, nullptr, &_imageView);
    VK_CHECK_ERR(result, "CustomImageView: failed to create image view!");
}

CustomImageView::CustomImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
        : _device(device) {
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &_imageView);
    VK_CHECK_ERR(result, "CustomImageView: failed to create image view!");
}

CustomImageView::~CustomImageView() {
    if (_device != VK_NULL_HANDLE && _imageView != VK_NULL_HANDLE) {
        vkDestroyImageView(_device, _imageView, nullptr);
    }
}

CustomImageView::CustomImageView(CustomImageView &&other)
        : _device(other._device),
        ImageView(other._imageView)
{
    other._device = VK_NULL_HANDLE;
    other._imageView = VK_NULL_HANDLE;
}