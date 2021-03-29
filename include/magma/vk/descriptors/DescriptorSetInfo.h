#pragma once
#include <vulkan/vulkan.hpp>
#include <list>
#include <vector>

class DescriptorSetInfo {
public:
    DescriptorSetInfo() = default;
    DescriptorSetInfo(const DescriptorSetInfo &) = delete;
    ~DescriptorSetInfo() = default;

    vk::DescriptorBufferInfo *newBufferInfo();
    vk::DescriptorImageInfo  *newImageInfo();
    vk::WriteDescriptorSet &newDescriptorWriteInfo();
    const std::vector<vk::WriteDescriptorSet> &getDescriptorWrites() const { return _descriptorWrites; }
    void clear();

private:
    std::list<vk::DescriptorBufferInfo> _bufferInfo;
    std::list<vk::DescriptorImageInfo>  _imageInfo;
    std::vector<vk::WriteDescriptorSet> _descriptorWrites;
};