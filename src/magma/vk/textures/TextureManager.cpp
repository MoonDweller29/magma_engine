#include "magma/vk/textures/TextureManager.h"

#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "magma/app/log.hpp"
#include "magma/app/image.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/vulkan_common.h"

TextureManager::TextureManager(LogicalDevice &device) 
        : _device(device),
        _commandBuffers(device.c_getDevice(), device.getGraphicsCmdPool(), 1)
{}

TextureManager::~TextureManager() {
    _commandBuffers.freeCmdBuf(_device.c_getDevice(), _device.getGraphicsCmdPool());
    if (_textures.size() > 0) {
        LOG_WARNING(_textures.size(), " textures haven't been removed");
        while(_textures.size() > 0) {
            deleteTexture(_textures.begin()->second);
        }
    }
}

bool TextureManager::textureExists(const std::string &name) const {
    return _textures.find(name) != _textures.end();
}

Texture &TextureManager::loadTexture(const std::string &texName, const std::string &path) {
    Image img(path.c_str(), 4);
    int imageSize = img.size();

    Buffer& stagingBuffer = _device.getBufferManager().createBufferWithData("stagingBuffer", img.data(), imageSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    Texture& texture = createTexture2D(texName,
        VK_FORMAT_R8G8B8A8_SRGB,
        VkExtent2D{(uint)img.getWidth(), (uint)img.getHeight()},
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_IMAGE_ASPECT_COLOR_BIT);
    setLayout(texture, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    copyBufToTex(texture, stagingBuffer.getBuf());
    setLayout(texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    _device.getBufferManager().deleteBuffer(stagingBuffer);
    return texture;
}

Texture& TextureManager::getTexture(const std::string &name) {
    if (!textureExists(name)) {
        LOG_AND_THROW std::invalid_argument(name + " texture not exist");
    }
    return _textures.at(name);
};

Texture& TextureManager::createTexture2D(const std::string &name, vk::Format format, vk::Extent2D extent,
        vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectMask) {
    if (textureExists(name)) {
        throw std::invalid_argument("texture " + name + " exist");
    }

    vk::ImageCreateInfo imageInfo{};
    imageInfo.imageType = vk::ImageType::e2D;
    imageInfo.format = format;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.samples = vk::SampleCountFlagBits::e1;
    imageInfo.tiling = vk::ImageTiling::eOptimal;
    imageInfo.usage = usage;
    imageInfo.sharingMode = vk::SharingMode::eExclusive;
    imageInfo.initialLayout = vk::ImageLayout::eUndefined;

    vk::Image textureImage;
    vk::Result result;
    std::tie(result, textureImage) = _device.getDevice().createImage(imageInfo, &textureImage);
    VK_HPP_CHECK_ERR(result, "Failed to create image!");

    vk::MemoryRequirements memRequirements;
    std::tie(result, memRequirements) = _device.getDevice().getImageMemoryRequirements(textureImage);
    vk::DeviceMemory textureMemory = _device.createDeviceMemory(memRequirements, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    _device.getDevice().bindImageMemory(textureImage, textureMemory);

    vk::ImageViewCreateInfo viewInfo{};
    viewInfo.image = textureImage;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

<<<<<<< HEAD
    VkImageView textureView;
    result = vkCreateImageView(_device.c_getDevice(), &viewInfo, nullptr, &textureView);
    VK_CHECK_ERR(result, "TextureManager::failed to create image view!");
=======
    vk::ImageView textureView;
    std::tie(result, textureView) = _device.getDevice().createImageView(viewInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create image view!");
>>>>>>> Init TextureManagerHpp

    TextureInfo* textureInfo = new TextureInfo;
    textureInfo->device = _device.c_getDevice();
    textureInfo->imageInfo = imageInfo;
    textureInfo->viewInfo = viewInfo;
    textureInfo->curLayout = vk::ImageLayout::eUndefined;
    textureInfo->name = name;

    _textures.emplace(name, Texture(textureImage, textureMemory, ImageView(textureView), textureInfo));
    return _textures.at(name);

}

[[depricated]] Texture& TextureManager::createTexture2D(const std::string &name, VkFormat c_format, VkExtent2D c_extent,
        VkImageUsageFlags c_usage, VkImageAspectFlags c_aspectMask) {
    vk::Format format(c_format);
    vk::Extent2D extent(c_extent);
    vk::ImageUsageFlags usage(c_usage);
    vk::ImageAspectFlags aspectMask(c_aspectMask);

    return createTexture2D(name, format, extent, usage, aspectMask);
}

static bool hasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void TextureManager::setLayout(Texture &texture, vk::ImageLayout newLayout) {
    vk::ImageCreateInfo imageInfo = texture.getInfo()->imageInfo;
    _commandBuffers.resetCmdBuf(0);
    VkCommandBuffer cmdBuf = _commandBuffers.beginCmdBuf(0);
    {
        VkImageLayout oldLayout = texture.getInfo()->curLayout;
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = texture.getImage();
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = imageInfo.mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = imageInfo.arrayLayers;

        VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(imageInfo.format))
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        switch (oldLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                barrier.srcAccessMask = 0;
                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            default:
                LOG_WARNING("Unsupported old image layout");
        }

        switch (newLayout) {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                barrier.dstAccessMask =
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                break;
            default:
                LOG_WARNING("Unsupported new image layout");
        }

        vkCmdPipelineBarrier(
                cmdBuf,
                srcStage, dstStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
        );
    }
    _commandBuffers.endAndSubmitCmdBuf_sync(0, _device.getGraphicsQueue());
    texture.getInfo()->curLayout = newLayout;
}

[[depricated]] void TextureManager::setLayout(Texture &texture, VkImageLayout newLayout) {
    VkImageCreateInfo imageInfo = texture.getInfo()->imageInfo;
    _commandBuffers.resetCmdBuf(0);
    VkCommandBuffer cmdBuf = _commandBuffers.beginCmdBuf(0);
    {
        VkImageLayout oldLayout = texture.getInfo()->curLayout;
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = texture.getImage();
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = imageInfo.mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = imageInfo.arrayLayers;

        VkPipelineStageFlags srcStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
        VkPipelineStageFlags dstStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(imageInfo.format))
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        switch (oldLayout) {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                barrier.srcAccessMask = 0;
                srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                break;
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            default:
                LOG_WARNING("Unsupported old image layout");
        }

        switch (newLayout) {
            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;
            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                break;
            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                barrier.dstAccessMask =
                        VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
                break;
            default:
                LOG_WARNING("Unsupported new image layout");
        }

        vkCmdPipelineBarrier(
                cmdBuf,
                srcStage, dstStage,
                0,
                0, nullptr,
                0, nullptr,
                1, &barrier
        );
    }
    _commandBuffers.endAndSubmitCmdBuf_sync(0, _device.getGraphicsQueue());
    texture.getInfo()->curLayout = newLayout;
}

void TextureManager::copyBufToTex(Texture &texture, VkBuffer buffer) {
    VkImageCreateInfo imageInfo = texture.getInfo()->imageInfo;
    _commandBuffers.resetCmdBuf(0);
    VkCommandBuffer cmdBuf = _commandBuffers.beginCmdBuf(0);
    {
        VkBufferImageCopy region{};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = imageInfo.arrayLayers;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = imageInfo.extent;

        vkCmdCopyBufferToImage(
                cmdBuf,
                buffer, texture.getImage(),
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &region
        );
    }
    _commandBuffers.endAndSubmitCmdBuf_sync(0, _device.getGraphicsQueue());
}

void TextureManager::deleteTexture(Texture &texture) {
     _textures.erase(texture.getInfo()->name);
    delete texture.getInfo();
    vkDestroyImageView(_device.c_getDevice(), texture.getView(), nullptr);
    vkDestroyImage(_device.c_getDevice(), texture.getImage(), nullptr);
    vkFreeMemory(_device.c_getDevice(), texture.getMemory(), nullptr);
}