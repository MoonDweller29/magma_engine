#pragma once

#include <vector>

#include "magma/vk/textures/TextureManager.h"
#include "magma/vk/vulkan_common.h"

class TexPyramid {
public:
    TexPyramid(const std::string &name, TextureManager &texMgr,
               vk::Extent2D baseExtent, int scaleFactor, int count,
               vk::Format format, vk::ImageUsageFlags usage, vk::ImageAspectFlags aspectMask);
    NON_COPYABLE(TexPyramid);
    ~TexPyramid();

    const Texture &operator[](int i) { return _textures[i];     }
    size_t         getSize()         { return _textures.size(); }

private:
    TextureManager      &_texMgr;
    std::vector<Texture> _textures;
};