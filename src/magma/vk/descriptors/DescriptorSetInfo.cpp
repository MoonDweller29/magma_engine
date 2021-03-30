#include "magma/vk/descriptors/DescriptorSetInfo.h"

vk::DescriptorBufferInfo *DescriptorSetInfo::newBufferInfo() {
    _bufferInfo.emplace_back(std::make_unique<vk::DescriptorBufferInfo>());
    return _bufferInfo.back().get();
}

vk::DescriptorImageInfo *DescriptorSetInfo::newImageInfo() {
    _imageInfo.emplace_back(std::make_unique<vk::DescriptorImageInfo>());
    return _imageInfo.back().get();
}

vk::WriteDescriptorSet &DescriptorSetInfo::newDescriptorWriteInfo() {
    _descriptorWrites.emplace_back();
    return _descriptorWrites.back();
}

void DescriptorSetInfo::clear() {
    _bufferInfo.clear();
    _imageInfo.clear();
    _descriptorWrites.clear();
}
