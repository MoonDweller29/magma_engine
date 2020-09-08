#include "descriptorSetLayout.h"
#include "vk/vulkan_common.h"

const VkDescriptorSetLayout &DescriptorSetLayout::createLayout(VkDevice device)
{
    this->device = device;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout);
    vk_check_err(result, "failed to create descriptor set layout!");

    return layout;
}

void DescriptorSetLayout::addUniformBuffer(uint32_t buf_size, VkShaderStageFlags stage_flags)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = bindings.size();
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = buf_size;
    uboLayoutBinding.stageFlags = stage_flags;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    bindings.push_back(uboLayoutBinding);
}

void DescriptorSetLayout::addCombinedImageSampler(VkShaderStageFlags stage_flags)
{
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = bindings.size();
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = stage_flags;

    bindings.push_back(samplerLayoutBinding);
}

void DescriptorSetLayout::clear()
{
    bindings.clear();
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
}