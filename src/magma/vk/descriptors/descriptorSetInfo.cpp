#include "magma/vk/descriptors/descriptorSetInfo.h"

VkDescriptorBufferInfo *DescriptorSetInfo::newBufferInfo()
{
    bufferInfo.push_back(VkDescriptorBufferInfo{});
    return &bufferInfo.back();
}

VkDescriptorImageInfo *DescriptorSetInfo::newImageInfo()
{
    imageInfo.push_back(VkDescriptorImageInfo{});
    return &imageInfo.back();
}

VkWriteDescriptorSet &DescriptorSetInfo::newDescriptorWriteInfo()
{
    descriptorWrites.push_back(VkWriteDescriptorSet{});
    return descriptorWrites[descriptorWrites.size() - 1];
}

void DescriptorSetInfo::clear()
{
    bufferInfo.clear();
    imageInfo.clear();
    descriptorWrites.clear();
}
