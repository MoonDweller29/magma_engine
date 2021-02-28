#include "magma/app/render/GBuffer.h"

#include "magma/vk/textures/TextureManager.h"

GBuffer::GBuffer(TextureManager &texMgr, vk::Extent2D extent) :
    _texMgr(texMgr),
    _depthTex(texMgr.createTexture2D("g_buffer_depth",
            texMgr.findDepthFormat(), extent,
            vk::ImageUsageFlagBits::eDepthStencilAttachment,
            vk::ImageAspectFlagBits::eDepth))
{}

GBuffer::~GBuffer() {
    _texMgr.deleteTexture(_depthTex);
}
