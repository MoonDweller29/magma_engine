#include "magma/app/image.h"

#include <algorithm>
#include <stdexcept>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include "magma/app/log.hpp"

struct StbDeleter {
    void operator()(stbi_uc* p) const {
//        printf("free of stb memory\n");
        stbi_image_free(p);
    }
};

Image::Image(const char *filename, int channel_count):
    depth(channel_count)
{
    int actualChCount;
    stbi_uc* pixels = stbi_load(filename, &width, &height, &actualChCount, channel_count);
    if (!pixels)
        LOG_AND_THROW std::runtime_error("failed to load image!");
    raw_data = std::shared_ptr<stbi_uc>(pixels, StbDeleter());
}

Image::Image(int w, int h, int ch):
    width(w), height(h), depth(ch)
{
    stbi_uc *mem = new stbi_uc[width*height*depth];
    raw_data = std::shared_ptr<stbi_uc>(mem);
}

void Image::save(std::string filename)
{
    int len = filename.size();
    if (strcmp(&(filename.data()[len - 3]), "png") == 0)
        stbi_write_png(filename.data(), width, height, depth, raw_data.get(), 0);
    else if (strcmp(&(filename.data()[len - 3]), "bmp") == 0)
        stbi_write_bmp(filename.data(), width, height, depth, raw_data.get());
    else if (strcmp(&(filename.data()[len - 3]), "tga") == 0)
        stbi_write_tga(filename.data(), width, height, depth, raw_data.get());
    else if (strcmp(&(filename.data()[len - 3]), "jpg") == 0)
        stbi_write_jpg(filename.data(), width, height, depth, raw_data.get(), 100);
    else
        printf("WRONG FORMAT\n");
}

stbi_uc &Image::operator()(int i, int j, int ch)
{
    ch = std::clamp(ch, 0, depth-1);
    i = std::clamp(i, 0, height-1);
    j = std::clamp(j, 0, width-1);

    return raw_data.get()[i * (width * depth) + j * depth + ch];
}
