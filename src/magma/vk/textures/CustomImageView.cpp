#include "magma/vk/textures/CustomImageView.h"

#include "magma/vk/vulkan_common.h"
#include <vulkan/vulkan.hpp>

CustomImageView::CustomImageView(Texture &texture, vk::ImageAspectFlags aspectMask) 
        : _device(texture.getInfo()->device) {
    vk::ImageCreateInfo info = texture.getInfo()->imageInfo;

    vk::ImageViewCreateInfo viewInfo({}, texture.getImage(), vk::ImageViewType::e2D, info.format, 
        {}, {aspectMask, 0, 1, 0, 1});

    vk::Result result;
    std::tie(result, _imageView) = _device.createImageView(viewInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create image view!");
}

CustomImageView::CustomImageView(vk::Device device, vk::Image image, vk::Format format, vk::ImageAspectFlags aspectMask)
        : _device(device) {
    vk::ImageViewCreateInfo viewInfo({}, image, vk::ImageViewType::e2D, format,
        {}, {aspectMask, 0, 1, 0, 1});

    vk::Result result;
    std::tie(result, _imageView) = _device.createImageView(viewInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create image view!");
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

[[depricated]] CustomImageView::CustomImageView(Texture &texture, VkImageAspectFlags c_aspectMask)
        : _device(texture.getInfo()->device) {
    vk::ImageCreateInfo info = texture.getInfo()->imageInfo;
    vk::ImageAspectFlags aspectMask(c_aspectMask);

    vk::ImageViewCreateInfo viewInfo({}, texture.getImage(), vk::ImageViewType::e2D, info.format, 
        {}, {aspectMask, 0, 1, 0, 1});

    vk::Result result;
    std::tie(result, _imageView) = _device.createImageView(viewInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create image view!");
}

[[depricated]] CustomImageView::CustomImageView(VkDevice c_device, VkImage c_image, VkFormat c_format, VkImageAspectFlags c_aspectMask)
        : _device(c_device) {
    vk::Device device(c_device);
    vk::Image image(c_image);
    vk::Format format(c_format);
    vk::ImageAspectFlags aspectMask(c_aspectMask);

    vk::ImageViewCreateInfo viewInfo({}, image, vk::ImageViewType::e2D, format,
        {}, {aspectMask, 0, 1, 0, 1});

    vk::Result result;
    std::tie(result, _imageView) = _device.createImageView(viewInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create image view!");
}