#pragma once
#include "magma/vk/textures/Texture.h"

class TextureManager;

class GBuffer {
public:
    GBuffer(TextureManager &texMgr, vk::Extent2D extent);
    ~GBuffer();

    vk::Extent2D getExtent() const { return _extent; }

    const Texture &getDepth()     const { return _depthTex;     }
    const Texture &getAlbedo()    const { return _albedoTex;    }
    const Texture &getNormals()   const { return _normalTex;    }
    const Texture &getGlobalPos() const { return _globalPosTex; }

private:
    Texture _depthTex;
    Texture _albedoTex;
    Texture _normalTex;
    Texture _globalPosTex;

    TextureManager &_texMgr;
    vk::Extent2D _extent;
};