/**
 * @file textureDump.h
 * @author Nastormo
 * @brief Cpp file that contains texture dump class
 * @version 0.1
 * @date 2021-01-24
 */
#include "app/textureDump.h"

#include <fstream>

void TextureDump::save(LogicalDevice &device, Texture &texture) {
    TextureInfo info = texture.getInfo();
    VkDeviceSize imgSize =  info.imageInfo.extent.height * info.imageInfo.extent.width * 4;

    Buffer dstBuffer = device.createBuffer(
        imgSize,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
    );

    device.transitionImageLayout(
            texture.img(), info.imageInfo.format,
            VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    SingleTimeCommandBuffer copyCmdBuf(device.handler(), device.getGraphicsCmdPool(), device.getGraphicsQueue());
    VkCommandBuffer copyCmd = copyCmdBuf.startRecording();
    {
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferImageHeight = 0;
        region.bufferRowLength = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = VkOffset3D{ 0, 0, 0 };
        region.imageExtent = VkExtent3D{ info.imageInfo.extent.width, info.imageInfo.extent.height, 1};

        vkCmdCopyImageToBuffer(
            copyCmd,
            texture.img(), 
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            dstBuffer.buf,
            1,
            &region
        );
    }
    copyCmdBuf.endRecordingAndSubmit();

    device.transitionImageLayout(
            texture.img(), info.imageInfo.format,
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    void *data;
    vkMapMemory(device.handler(), dstBuffer.mem, 0, imgSize, 0, &data);


    std::ofstream file("tmp.txt", std::ios::out | std::ios::binary);

    // ppm header
    file << "P6\n" << info.imageInfo.extent.width << "\n" << info.imageInfo.extent.height << "\n" << 255 << "\n";

    float *row = (float*)data;

    auto size_v = info.imageInfo.extent.width * info.imageInfo.extent.height;

    for (uint32_t y = 0; y < size_v; y++) {

        file.write((char*)row + 1, 1);
        file.write((char*)row + 1, 1);
        file.write((char*)row + 1, 1);

        row++;

    }

    file.close();
    device.deleteBuffer(dstBuffer);
}

