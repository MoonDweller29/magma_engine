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

    void addUniformBuffer(uint32_t bufSize, vk::ShaderStageFlags stageFlags);
    void addCombinedImageSampler(vk::ShaderStageFlags stageFlags);
    void addStorageImage(vk::ShaderStageFlags stageFlags);
private:
    std::vector<vk::DescriptorSetLayoutBinding>      _bindings;
    std::unordered_map<vk::DescriptorType, uint32_t> _poolSizes;

    void increaseDescriptorsCount(vk::DescriptorType descrType, int descrCount);
};