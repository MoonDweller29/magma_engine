#pragma once
#include <stb_image/stb_image.h>
#include <memory>
#include <string>

class Image
{
private:
    std::shared_ptr<stbi_uc> data; //raw_data
    int width, height;
    int depth; //channel count
public:
    Image(const char *filename, int channel_count);
    Image(const Image &other) = default;
    Image(int w, int h, int ch);

    void save(std::string filename);
    stbi_uc &operator()(int i, int j, int ch=0);

    ~Image() = default;
};