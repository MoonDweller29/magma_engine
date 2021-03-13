#include "magma/vk/textures/CustomImageView.h"

#include <vulkan/vulkan.hpp>

#include "magma/vk/vulkan_common.h"

CustomImageView::CustomImageView(Texture &texture, vk::ImageAspectFlags aspectMask)
        : _device(texture.getInfo()->device) {
    vk::ImageCreateInfo info = texture.getInfo()->imageInfo;

    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = texture.getImage();
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = info.format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk::Result result;
    std::tie(result, _imageView) = _device.createImageView(viewInfo);
    VK_CHECK_ERR(result, "Failed to create image view!");
}

CustomImageView::CustomImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectMask)
        : _device(device) {

    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = image;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk::Result result;
    std::tie(result, _imageView) = _device.createImageView(viewInfo);
    VK_CHECK_ERR(result, "Failed to create image view!");
}

CustomImageView::~CustomImageView() {
    if (_device && _imageView) {
        _device.destroyImageView(_imageView);
    }
}

CustomImageView::CustomImageView(CustomImageView &&other)
        : _device(other._device),
        ImageView(other._imageView)
{
    other._device = vk::Device();
    other._imageView = vk::ImageView();
}

[[deprecated]] CustomImageView::CustomImageView(Texture &texture, VkImageAspectFlags c_aspectMask)
        : CustomImageView(texture, vk::ImageAspectFlags(c_aspectMask))
{}

[[deprecated]] CustomImageView::CustomImageView(VkDevice c_device, VkImage c_image, VkFormat c_format, VkImageAspectFlags c_aspectMask)
        : CustomImageView(vk::Device(c_device), vk::Image(c_image), vk::Format(c_format), vk::ImageAspectFlags(c_aspectMask))
{}
