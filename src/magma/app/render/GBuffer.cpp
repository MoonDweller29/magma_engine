#include "magma/app/render/GBuffer.h"

#include "magma/vk/textures/TextureManager.h"

GBuffer::GBuffer(TextureManager &texMgr, vk::Extent2D extent) :
    _texMgr(texMgr), _extent(extent),
    _depthTex(texMgr.createTexture2D("g_buffer_depth",
            texMgr.findDepthFormat(), extent,
            vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eSampled,
            vk::ImageAspectFlagBits::eDepth)),
    _albedoTex(texMgr.createTexture2D("g_buffer_albedo",
               vk::Format::eR16G16B16A16Unorm, extent,
               vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
               vk::ImageAspectFlagBits::eColor)),
    _normalTex(texMgr.createTexture2D("g_buffer_normals",
               vk::Format::eR16G16B16A16Snorm, extent,
               vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
               vk::ImageAspectFlagBits::eColor)),
    _globalPosTex(texMgr.createTexture2D("g_buffer_global_pos",
                 vk::Format::eR16G16B16A16Sfloat, extent,
                 vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
                 vk::ImageAspectFlagBits::eColor))
{}

GBuffer::~GBuffer() {
    _texMgr.deleteTexture(_depthTex);
    _texMgr.deleteTexture(_albedoTex);
    _texMgr.deleteTexture(_normalTex);
    _texMgr.deleteTexture(_globalPosTex);
}
