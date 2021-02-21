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
        vk::Format::eR8G8B8A8Srgb,
        vk::Extent2D{(uint)img.getWidth(), (uint)img.getHeight()},
        vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
        vk::ImageAspectFlagBits::eColor);
    setLayout(texture, vk::ImageLayout::eTransferDstOptimal);
    copyBufToTex(texture, stagingBuffer.getBuf());
    setLayout(texture, vk::ImageLayout::eShaderReadOnlyOptimal);

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
        LOG_AND_THROW std::invalid_argument("texture " + name + " exist");
    }

    vk::ImageCreateInfo imageInfo;
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

    auto [result, textureImage] = _device.getDevice().createImage(imageInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create image!");

    vk::MemoryRequirements memRequirements = _device.getDevice().getImageMemoryRequirements(textureImage);
    vk::DeviceMemory textureMemory = _device.memAlloc(memRequirements, vk::MemoryPropertyFlagBits::eDeviceLocal);

    result = _device.getDevice().bindImageMemory(textureImage, textureMemory, 0);
    VK_HPP_CHECK_ERR(result, "Failed to bind image memory!");

    vk::ImageViewCreateInfo viewInfo;
    viewInfo.image = textureImage;
    viewInfo.viewType = vk::ImageViewType::e2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectMask;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    vk::ImageView textureView;
    std::tie(result, textureView) = _device.getDevice().createImageView(viewInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create image view!");

    TextureInfo* textureInfo = new TextureInfo;
    textureInfo->device = _device.c_getDevice();
    textureInfo->imageInfo = imageInfo;
    textureInfo->viewInfo = viewInfo;
    textureInfo->curLayout = vk::ImageLayout::eUndefined;
    textureInfo->name = name;

    _textures.emplace(name, Texture(textureImage, textureMemory, ImageView(textureView), textureInfo));
    return _textures.at(name);

}

static bool hasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

/** @todo remove c comand Buffer **/
void TextureManager::setLayout(Texture &texture, vk::ImageLayout newLayout) {
    vk::ImageCreateInfo imageInfo = texture.getInfo()->imageInfo;
    _commandBuffers.resetCmdBuf(0);
    vk::CommandBuffer cmdBuf = vk::CommandBuffer(_commandBuffers.beginCmdBuf(0));
    {
        vk::ImageLayout oldLayout = texture.getInfo()->curLayout;

        vk::ImageMemoryBarrier barrier;
        barrier.oldLayout = oldLayout;
        barrier.newLayout = newLayout;
        barrier.image = texture.getImage();
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = imageInfo.mipLevels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = imageInfo.arrayLayers;

        vk::PipelineStageFlags srcStage = vk::PipelineStageFlagBits::eAllCommands;
        vk::PipelineStageFlags dstStage = vk::PipelineStageFlagBits::eAllCommands;

        if (newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;

            if (hasStencilComponent(imageInfo.format))
                barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        } else {
            barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        }

        switch (oldLayout) {
            case vk::ImageLayout::eUndefined:
                srcStage = vk::PipelineStageFlagBits::eTopOfPipe;
                break;
            case vk::ImageLayout::eTransferDstOptimal:
                barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                srcStage = vk::PipelineStageFlagBits::eTransfer;
                break;
            default:
                LOG_WARNING("Unsupported old image layout");
        }

        switch (newLayout) {
            case vk::ImageLayout::eTransferDstOptimal:
                barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
                dstStage = vk::PipelineStageFlagBits::eTransfer;
                break;
            case vk::ImageLayout::eShaderReadOnlyOptimal:
                barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
                dstStage = vk::PipelineStageFlagBits::eFragmentShader;
                break;
            case vk::ImageLayout::eDepthStencilAttachmentOptimal:
                barrier.dstAccessMask =
                        vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                dstStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
                break;
            default:
                LOG_WARNING("Unsupported new image layout");
        }

        cmdBuf.pipelineBarrier(srcStage, dstStage, {}, nullptr, nullptr, barrier);
    }
    _commandBuffers.endAndSubmitCmdBuf_sync(0, _device.getGraphicsQueue());
    texture.getInfo()->curLayout = newLayout;
}

[[deprecated]] void TextureManager::setLayout(Texture &texture, VkImageLayout c_newLayout) {
    setLayout(texture, vk::ImageLayout(c_newLayout));
}

/** @todo remove c comand Buffer **/
void TextureManager::copyBufToTex(Texture &texture, vk::Buffer buffer) {
    vk::ImageCreateInfo imageInfo = texture.getInfo()->imageInfo;
    _commandBuffers.resetCmdBuf(0);
    vk::CommandBuffer cmdBuf = vk::CommandBuffer(_commandBuffers.beginCmdBuf(0));
    {
        vk::BufferImageCopy region;
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;

        region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = imageInfo.arrayLayers;

        region.imageOffset = vk::Offset3D{0, 0, 0};
        region.imageExtent = imageInfo.extent;

        cmdBuf.copyBufferToImage(buffer, texture.getImage(), vk::ImageLayout::eTransferDstOptimal, region);
    }
    _commandBuffers.endAndSubmitCmdBuf_sync(0, _device.getGraphicsQueue());
}

[[deprecated]] void TextureManager::copyBufToTex(Texture &texture, VkBuffer c_buffer) {
    copyBufToTex(texture, vk::Buffer(c_buffer));
}

void TextureManager::deleteTexture(Texture &texture) {
     _textures.erase(texture.getInfo()->name);
    delete texture.getInfo();
    vk::Device device = _device.getDevice();
    device.destroyImageView(texture.getView());
    device.destroyImage(texture.getImage());
    device.freeMemory(texture.getMemory());
}