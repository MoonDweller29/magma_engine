#include "image.h"
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>

struct StbDeleter {
    void operator()(stbi_uc* p) const {
//        printf("free of stb memory\n");
        stbi_image_free(p);
    }
};

Image::Image(const char *filename, int channel_count)
{
    data = std::shared_ptr<stbi_uc>(
            stbi_load(filename, &width, &height, &depth, channel_count),
            StbDeleter()
    );
}

Image::Image(int w, int h, int ch):
    width(w), height(h), depth(ch)
{
    stbi_uc *mem = new stbi_uc[width*height*depth];
    data = std::shared_ptr<stbi_uc>(mem);
}

void Image::save(std::string filename)
{
    int len = filename.size();
    if (strcmp(&(filename.data()[len - 3]), "png") == 0)
        stbi_write_png(filename.data(), width, height, depth, data.get(), 0);
    else if (strcmp(&(filename.data()[len - 3]), "bmp") == 0)
        stbi_write_bmp(filename.data(), width, height, depth, data.get());
    else if (strcmp(&(filename.data()[len - 3]), "tga") == 0)
        stbi_write_tga(filename.data(), width, height, depth, data.get());
    else if (strcmp(&(filename.data()[len - 3]), "jpg") == 0)
        stbi_write_jpg(filename.data(), width, height, depth, data.get(), 100);
    else
        printf("WRONG FORMAT\n");
}

stbi_uc &Image::operator()(int i, int j, int ch)
{
    ch = std::clamp(ch, 0, depth-1);
    i = std::clamp(i, 0, height-1);
    j = std::clamp(j, 0, width-1);

    return data.get()[i*(width*depth) + j*depth + ch];
}