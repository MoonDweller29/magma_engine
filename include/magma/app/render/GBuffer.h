#pragma once
#include "magma/vk/textures/Texture.h"

class TextureManager;

class GBuffer {
public:
    GBuffer(TextureManager &texMgr, vk::Extent2D extent);
    ~GBuffer();

    const Texture &getDepth() const { return _depthTex; }

private:
    Texture _depthTex;

    TextureManager &_texMgr;
};