#pragma once
#include <stb_image/stb_image.h>
#include <memory>
#include <string>

class Image
{
private:
    std::shared_ptr<stbi_uc> raw_data; //raw_data
    int width, height;
    int depth; //channel count
public:
    Image(const char *filename, int channel_count);
    Image(const Image &other) = default;
    Image(int w, int h, int ch);

    const stbi_uc *data() const { return raw_data.get(); }
    int size() const { return width*height*depth; }

    void save(std::string filename);
    stbi_uc &operator()(int i, int j, int ch=0);

    const int &getWidth() const { return width; }
    const int &getHeight() const { return height; }
    const int &getChCount() const { return depth; }


    ~Image() = default;
};