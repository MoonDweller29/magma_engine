#include "magma/vk/textures/TextureManager.h"

#include <stdexcept>
#include <vulkan/vulkan.hpp>

#include "magma/app/log.hpp"
#include "magma/app/image.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/vulkan_common.h"

TextureManager::TextureManager(LogicalDevice &device)
        : _device(device),
        _commandBuffer(device.getDevice(), device.getGraphicsQueue().cmdPool)
{}

TextureManager::~TextureManager() {
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
        vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    Texture& texture = createTexture2D(texName,
        vk::Format::eR8G8B8A8Srgb,
        vk::Extent2D{(uint32_t)img.getWidth(), (uint32_t)img.getHeight()},
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
    VK_CHECK_ERR(result, "Failed to create image!");

    vk::MemoryRequirements memRequirements = _device.getDevice().getImageMemoryRequirements(textureImage);
    vk::DeviceMemory textureMemory = _device.memAlloc(memRequirements, vk::MemoryPropertyFlagBits::eDeviceLocal);

    result = _device.getDevice().bindImageMemory(textureImage, textureMemory, 0);
    VK_CHECK_ERR(result, "Failed to bind image memory!");

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
    VK_CHECK_ERR(result, "Failed to create image view!");

    TextureInfo* textureInfo = new TextureInfo;
    textureInfo->device = _device.getDevice();
    textureInfo->imageInfo = imageInfo;
    textureInfo->viewInfo = viewInfo;
    textureInfo->curLayout = vk::ImageLayout::eUndefined;
    textureInfo->name = name;

#ifndef NDEBUG
    // set the name
    vk::DebugUtilsObjectNameInfoEXT nameInfo;
    nameInfo.objectType = vk::ObjectType::eImage;
    nameInfo.objectHandle = (uint64_t)(VkImage)textureImage;
    nameInfo.pObjectName = name.c_str();
    result = _device.getDevice().setDebugUtilsObjectNameEXT(nameInfo);
    VK_CHECK_ERR(result, "Failed to set image name!");
#endif

    _textures.emplace(name, Texture(textureImage, textureMemory, ImageView(textureView), textureInfo));
    return _textures.at(name);
}

static bool hasStencilComponent(vk::Format format) {
    return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

void TextureManager::setLayout(Texture &texture, vk::ImageLayout newLayout) {
    vk::ImageCreateInfo imageInfo = texture.getInfo()->imageInfo;
    _commandBuffer.reset();
    vk::CommandBuffer cmdBuf = _commandBuffer.begin();
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
    _commandBuffer.endAndSubmit_sync(_device.getGraphicsQueue().queue);
    texture.getInfo()->curLayout = newLayout;
}

void TextureManager::copyBufToTex(Texture &texture, vk::Buffer buffer) {
    vk::ImageCreateInfo imageInfo = texture.getInfo()->imageInfo;
    _commandBuffer.reset();
    vk::CommandBuffer cmdBuf = _commandBuffer.begin();
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
    _commandBuffer.endAndSubmit_sync(_device.getGraphicsQueue().queue);
}

vk::Format TextureManager::findSupportedFormat(
        const std::vector<vk::Format>& candidates,
        vk::ImageTiling tiling, vk::FormatFeatureFlags features
) {
    for (vk::Format format : candidates) {
        vk::FormatProperties props = _device.getPhysDevice().getDevice().getFormatProperties(format);
        if (tiling == vk::ImageTiling::eLinear && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == vk::ImageTiling::eOptimal && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    LOG_AND_THROW std::runtime_error("failed to find supported format!");
}

vk::Format TextureManager::findDepthFormat() {
    return findSupportedFormat(
            {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
            vk::ImageTiling::eOptimal,
            vk::FormatFeatureFlagBits::eDepthStencilAttachment
    );
}

void TextureManager::deleteTexture(Texture &texture) {
     _textures.erase(texture.getInfo()->name);
    delete texture.getInfo();
    vk::Device device = _device.getDevice();
    device.destroyImageView(texture.getView());
    device.destroyImage(texture.getImage());
    device.freeMemory(texture.getMemory());
}
