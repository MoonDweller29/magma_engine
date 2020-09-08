#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class DescriptorSetLayout
{
    VkDescriptorSetLayout layout = VK_NULL_HANDLE;
    VkDevice device = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayoutBinding> bindings;
public:
    DescriptorSetLayout() = default;
    ~DescriptorSetLayout() = default;

    const VkDescriptorSetLayout &createLayout(VkDevice device);
    void clear();

    const size_t size() const { return bindings.size(); }
    const VkDescriptorSetLayout &getLayout() const { return layout; }

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // adding bindings
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void addUniformBuffer(uint32_t buf_size, VkShaderStageFlags stage_flags);
    void addCombinedImageSampler(VkShaderStageFlags stage_flags);
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////
};