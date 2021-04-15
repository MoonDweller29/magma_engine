#include "magma/vk/textures/TexPyramid.h"


TexPyramid::TexPyramid(const std::string &name, TextureManager &texMgr,
           vk::Extent2D baseExtent, int scaleFactor, int count,
           vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectMask) :
    _texMgr(texMgr)
{
    int width = baseExtent.width;
    int height = baseExtent.height;

    for (int i = 0; i < count; ++i) {
        std::string texName = name + "_" + std::to_string(i);
        vk::Extent2D extent(width, height);
        _textures.push_back(_texMgr.createTexture2D(texName, format, extent, usage, aspectMask));

        width  = (width  + scaleFactor - 1) / scaleFactor;
        height = (height + scaleFactor - 1) / scaleFactor;
    }
}

TexPyramid::~TexPyramid() {
    for (auto &tex : _textures) {
        _texMgr.deleteTexture(tex);
    }
}