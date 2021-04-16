#include "magma/vk/textures/CombinedImageSampler.h"

CombinedImageSampler::CombinedImageSampler(vk::ImageView view, vk::Sampler sampler) :
    _view(view), _sampler(sampler)
{}
