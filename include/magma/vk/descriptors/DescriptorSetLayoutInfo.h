#pragma once
#include <vector>
#include <unordered_map>
#include <vulkan/vulkan.hpp>

class DescriptorSetLayoutInfo {
public:
    DescriptorSetLayoutInfo() = default;
    ~DescriptorSetLayoutInfo() = default;

    const std::vector<vk::DescriptorSetLayoutBinding>      &getBindings()  const { return _bindings;  }
    const std::unordered_map<vk::DescriptorType, uint32_t> &getPoolSizes() const { return _poolSizes; }

    DescriptorSetLayoutInfo &addUniformBuffer(uint32_t bufSize, vk::ShaderStageFlags stageFlags);
    DescriptorSetLayoutInfo &addCombinedImageSampler(vk::ShaderStageFlags stageFlags);
    DescriptorSetLayoutInfo &addStorageImage(vk::ShaderStageFlags stageFlags);
private:
    std::vector<vk::DescriptorSetLayoutBinding>      _bindings;
    std::unordered_map<vk::DescriptorType, uint32_t> _poolSizes;

    void increaseDescriptorsCount(vk::DescriptorType descrType, int descrCount);
};