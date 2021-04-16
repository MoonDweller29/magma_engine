#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <list>
#include <memory>

#include "magma/vk/vulkan_common.h"

class DescriptorSetInfo {
public:
    DescriptorSetInfo() = default;
    NON_COPYABLE(DescriptorSetInfo);
    ~DescriptorSetInfo() = default;

    vk::DescriptorBufferInfo *newBufferInfo();
    vk::DescriptorImageInfo  *newImageInfo();
    std::vector<vk::DescriptorImageInfo> &newImageArrayInfo(int size);
    vk::WriteDescriptorSet &newDescriptorWriteInfo();
    const std::vector<vk::WriteDescriptorSet> &getDescriptorWrites() const { return _descriptorWrites; }
    void clear();

private:
    std::vector<std::unique_ptr<vk::DescriptorBufferInfo>> _bufferInfo;
    std::vector<std::unique_ptr<vk::DescriptorImageInfo>>  _imageInfo;
    std::list<std::vector<vk::DescriptorImageInfo>>        _imageArrayInfo;
    std::vector<vk::WriteDescriptorSet> _descriptorWrites;
};