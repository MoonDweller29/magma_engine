#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>
#include <unordered_map>

#include "magma/vk/descriptors/DescriptorPool.h"
#include "magma/vk/descriptors/DescriptorSetInfo.h"
#include "magma/vk/descriptors/DescriptorSetLayoutInfo.h"

class DescriptorSetLayout {
public:
    DescriptorSetLayout(vk::Device device, const DescriptorSetLayoutInfo &layoutInfo);
    DescriptorSetLayout(const DescriptorSetLayout &) = delete;
    ~DescriptorSetLayout();

    void freePool();

    const vk::DescriptorSetLayout &getLayout() const { return _layout; }

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
    DescriptorSetLayoutInfo _layoutInfo;
    vk::DescriptorSetLayout _layout;

    std::vector<DescriptorPool> _pools;
    std::vector<vk::DescriptorSet> _descriptorSets;
    DescriptorSetInfo _descriptorSetInfo;
    uint32_t _setInd; //index of set that is recording

    void checkBindingIndex(uint32_t bindingInd, vk::DescriptorType descriptorType);
};