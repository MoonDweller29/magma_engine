#include "magma/vk/textures/ImageView.h"

ImageView::ImageView() 
    : _imageView(VK_NULL_HANDLE)
{}

ImageView::ImageView(VkImageView imageView)
    : _imageView(imageView)
{}
