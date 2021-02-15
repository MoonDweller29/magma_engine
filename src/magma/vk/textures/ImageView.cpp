/**
 * @file ImageView.cpp
 * @brief Class contain image view
 * @version 0.1
 * @date 2021-02-12
 */
#include "magma/vk/textures/ImageView.h"

ImageView::ImageView() 
    : _imageView(VK_NULL_HANDLE)
{}

ImageView::ImageView(VkImageView imageView)
    : _imageView(imageView)
{}
