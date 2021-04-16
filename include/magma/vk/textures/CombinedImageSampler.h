#pragma once

#include <vulkan/vulkan.hpp>

class CombinedImageSampler {
public:
    CombinedImageSampler(vk::ImageView view, vk::Sampler sampler);

    const vk::ImageView &getImageView() const  { return _view;    }
    const vk::Sampler   &getSampler()   const  { return _sampler; }
private:
    vk::ImageView _view;
    vk::Sampler   _sampler;
};