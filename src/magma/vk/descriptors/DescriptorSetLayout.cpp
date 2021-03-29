#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/vulkan_common.h"
#include <sstream>

void DescriptorSetLayout::clearPoolSizes()
{
    poolSizes.clear();
}

DescriptorSetLayout::DescriptorSetLayout():
        layout(VK_NULL_HANDLE), device(VK_NULL_HANDLE), setInd(0)
{
    clearPoolSizes();
}


const VkDescriptorSetLayout &DescriptorSetLayout::createLayout(VkDevice device)
{
    this->device = device;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();

    VkResult result = vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &layout);
    VK_CHECK_ERR(result, "failed to create descriptor set layout!");

    pools.push_back(DescriptorPool(device, poolSizes, DescriptorPool::DEFAULT_SET_COUNT));

    return layout;
}

void DescriptorSetLayout::increaseDescriptorsCount(vk::DescriptorType desc_type, int desc_count)
{
    if (poolSizes.find(desc_type) != poolSizes.end()) {
        poolSizes[desc_type] += desc_count;
    } else {
        poolSizes[desc_type] = desc_count;
    }
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
    increaseDescriptorsCount(vk::DescriptorType::eUniformBuffer, buf_size);
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
    increaseDescriptorsCount(vk::DescriptorType::eCombinedImageSampler, 1);
}

void DescriptorSetLayout::addStorageImage(VkShaderStageFlags stage_flags) {
    VkDescriptorSetLayoutBinding storageImageLayoutBinding{};
    storageImageLayoutBinding.binding = bindings.size();
    storageImageLayoutBinding.descriptorCount = 1;
    storageImageLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    storageImageLayoutBinding.pImmutableSamplers = nullptr;
    storageImageLayoutBinding.stageFlags = stage_flags;

    bindings.push_back(storageImageLayoutBinding);
    increaseDescriptorsCount(vk::DescriptorType::eStorageImage, 1);
}

void DescriptorSetLayout::freePool()
{
    pools.clear();
    pools.push_back(DescriptorPool(device, poolSizes, DescriptorPool::DEFAULT_SET_COUNT));
}

void DescriptorSetLayout::clear()
{
    pools.clear();
    clearPoolSizes();
    bindings.clear();
    vkDestroyDescriptorSetLayout(device, layout, nullptr);
}

void DescriptorSetLayout::allocateSets(uint32_t count)
{
    descriptorSetInfo.clear();
    descriptorSets.clear();
    //iteration over existing pools
    for (uint32_t i = 0; i < pools.size() && count > 0; ++i)
    {
        if (pools[i].isFull())
            continue;
        auto newSets = pools[i].allocateSets(layout, count);
        count -= newSets.size();
        descriptorSets.insert(descriptorSets.end(), newSets.begin(), newSets.end());
    }
    //allocation new pools if needed
    while (count > 0)
    {
        pools.push_back(DescriptorPool(device, poolSizes, DescriptorPool::DEFAULT_SET_COUNT));
        auto newSets = pools.back().allocateSets(layout, count);
        count -= newSets.size();
        descriptorSets.insert(descriptorSets.end(), newSets.begin(), newSets.end());
    }
}

void DescriptorSetLayout::beginSet(uint32_t ind)
{
    if (ind > descriptorSets.size())
        LOG_AND_THROW std::runtime_error("beginSet: wrong descriptorSet index");
    setInd = ind;
}

void DescriptorSetLayout::bindUniformBuffer(uint32_t binding, VkBuffer buf, VkDeviceSize offset, VkDeviceSize range)
{
    if (bindings[binding].descriptorType != VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
    {
        std::stringstream message;
        message << "bindUniformBuffer: binding index mismatch:\n binding <" << binding <<
        "> has type <" << bindings[binding].descriptorType << "> which is not VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER";
        LOG_AND_THROW std::runtime_error(message.str());
    }

    VkDescriptorBufferInfo *bufferInfo = descriptorSetInfo.newBufferInfo();
    bufferInfo->buffer = buf;
    bufferInfo->offset = offset;
    bufferInfo->range = range; //size of all buffer (not the size of one elem)

    VkWriteDescriptorSet &descriptorWrite = descriptorSetInfo.newDescriptorWriteInfo();

    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets[setInd];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0; //first index in array
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorWrite.descriptorCount = bindings[binding].descriptorCount;
    descriptorWrite.pBufferInfo = bufferInfo;
}

void DescriptorSetLayout::bindCombinedImageSampler(uint32_t binding, VkImageView imageView, VkSampler sampler)
{
    if (bindings[binding].descriptorType != VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
    {
        std::stringstream message;
        message << "bindCombinedImageSampler: binding index mismatch:\n binding <" << binding <<
                "> has type <" << bindings[binding].descriptorType << "> which is not VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER";
        LOG_AND_THROW std::runtime_error(message.str());
    }

    VkDescriptorImageInfo *imageInfo = descriptorSetInfo.newImageInfo();
    imageInfo->imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo->imageView = imageView;
    imageInfo->sampler = sampler;

    VkWriteDescriptorSet &descriptorWrite = descriptorSetInfo.newDescriptorWriteInfo();

    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets[setInd];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = imageInfo;
}

void DescriptorSetLayout::bindStorageImage(uint32_t binding, VkImageView imageView, VkImageLayout imageLayout)
{
    if (bindings[binding].descriptorType != VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)
    {
        std::stringstream message;
        message << "bindStorageImage: binding index mismatch:\n binding <" << binding <<
                "> has type <" << bindings[binding].descriptorType << "> which is not VK_DESCRIPTOR_TYPE_STORAGE_IMAGE";
        LOG_AND_THROW std::runtime_error(message.str());
    }

    VkDescriptorImageInfo *imageInfo = descriptorSetInfo.newImageInfo();
    imageInfo->imageLayout = imageLayout;
    imageInfo->imageView = imageView;
    imageInfo->sampler = VK_NULL_HANDLE;

    VkWriteDescriptorSet &descriptorWrite = descriptorSetInfo.newDescriptorWriteInfo();

    descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet = descriptorSets[setInd];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = imageInfo;
}

std::vector<VkDescriptorSet> DescriptorSetLayout::recordAndReturnSets()
{
    const std::vector<VkWriteDescriptorSet> &descriptorWrites = descriptorSetInfo.getDescriptorWrites();
    vkUpdateDescriptorSets(device,
                           static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(),
                           0, nullptr);

    return descriptorSets;
}
