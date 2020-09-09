#pragma once
#include <vulkan/vulkan.h>
#include <list>
#include <vector>

class DescriptorSetInfo
{
    std::list<VkDescriptorBufferInfo> bufferInfo;
    std::list<VkDescriptorImageInfo> imageInfo;
    std::vector<VkWriteDescriptorSet> descriptorWrites;
public:
    DescriptorSetInfo() = default;
    ~DescriptorSetInfo() = default;

    VkDescriptorBufferInfo *newBufferInfo();
    VkDescriptorImageInfo *newImageInfo();
    VkWriteDescriptorSet &newDescriptorWriteInfo();
    const std::vector<VkWriteDescriptorSet> &getDescriptorWrites() const { return descriptorWrites; }
    void clear();
};