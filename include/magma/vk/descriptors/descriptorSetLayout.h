#pragma once
#include "descriptorPool.h"
#include "descriptorSetInfo.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <unordered_map>

class DescriptorSetLayout
{
    VkDescriptorSetLayout layout;
    VkDevice device;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
    std::unordered_map<VkDescriptorType, uint32_t> poolSizes;
    std::vector<DescriptorPool> pools;

    void clearPoolSizes();
    void increaseDescriptorsCount(VkDescriptorType desc_type, int desc_count);

    std::vector<VkDescriptorSet> descriptorSets;
    DescriptorSetInfo descriptorSetInfo;
    uint32_t setInd; //index of set that is recording
public:
    DescriptorSetLayout();
    ~DescriptorSetLayout() = default;

    const VkDescriptorSetLayout &createLayout(VkDevice device);
    void freePool();
    void clear();

    const size_t size() const { return bindings.size(); }
    const VkDescriptorSetLayout &getLayout() const { return layout; }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // adding bindings to layout
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void addUniformBuffer(uint32_t buf_size, VkShaderStageFlags stage_flags);
    void addCombinedImageSampler(VkShaderStageFlags stage_flags);
    void addStorageImage(VkShaderStageFlags stage_flags);
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // descriptorSet recording
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void allocateSets(uint32_t count);
    void beginSet(uint32_t ind);
    void bindUniformBuffer(uint32_t binding, VkBuffer buf, VkDeviceSize offset, VkDeviceSize range);
    void bindCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler, VkImageLayout imageLayout);
    void bindStorageImage(uint32_t binding, VkImageView imageView, VkSampler sampler, VkImageLayout imageLayout);
    std::vector<VkDescriptorSet> recordAndReturnSets();
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////
};