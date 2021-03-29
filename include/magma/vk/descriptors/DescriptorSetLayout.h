#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_map>

#include "DescriptorPool.h"
#include "DescriptorSetInfo.h"

class DescriptorSetLayout {
public:
    DescriptorSetLayout();
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    ~DescriptorSetLayout();

    void createLayout(vk::Device device);
    void freePool();

    const vk::DescriptorSetLayout &getLayout() const { return _layout; }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // adding bindings to layout
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void addUniformBuffer(uint32_t bufSize, vk::ShaderStageFlags stageFlags);
    void addCombinedImageSampler(vk::ShaderStageFlags stageFlags);
    void addStorageImage(vk::ShaderStageFlags stageFlags);
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // descriptorSet recording
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void allocateSets(uint32_t count);
    void beginSet(uint32_t ind);
    void bindUniformBuffer(uint32_t binding, vk::Buffer buf, vk::DeviceSize offset, vk::DeviceSize range);
    void bindCombinedImageSampler(uint32_t binding, vk::ImageView imageView, vk::Sampler sampler);
    void bindStorageImage(uint32_t binding, vk::ImageView imageView, vk::ImageLayout imageLayout);
    std::vector<vk::DescriptorSet> recordAndReturnSets();
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    vk::Device _device;
    std::vector<vk::DescriptorSetLayoutBinding> _bindings;
    std::unordered_map<vk::DescriptorType, uint32_t> _poolSizes;
    vk::DescriptorSetLayout _layout;

    std::vector<DescriptorPool> _pools;

    void increaseDescriptorsCount(vk::DescriptorType descrType, int descrCount);

    std::vector<vk::DescriptorSet> _descriptorSets;
    DescriptorSetInfo _descriptorSetInfo;
    uint32_t _setInd; //index of set that is recording
};