#include "magma/vk/descriptors/DescriptorSetInfo.h"

vk::DescriptorBufferInfo *DescriptorSetInfo::newBufferInfo() {
    _bufferInfo.emplace_back();
    return &_bufferInfo.back();
}

vk::DescriptorImageInfo *DescriptorSetInfo::newImageInfo() {
    _imageInfo.emplace_back();
    return &_imageInfo.back();
}

vk::WriteDescriptorSet &DescriptorSetInfo::newDescriptorWriteInfo() {
    _descriptorWrites.emplace_back();
    return _descriptorWrites[_descriptorWrites.size() - 1]; //@TODO: change to method back
}

void DescriptorSetInfo::clear() {
    _bufferInfo.clear();
    _imageInfo.clear();
    _descriptorWrites.clear();
}
