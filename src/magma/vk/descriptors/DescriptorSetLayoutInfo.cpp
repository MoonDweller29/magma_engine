#include "magma/vk/descriptors/DescriptorSetLayoutInfo.h"


DescriptorSetLayoutInfo &DescriptorSetLayoutInfo::addUniformBuffer(uint32_t bufSize, vk::ShaderStageFlags stageFlags) {
    vk::DescriptorSetLayoutBinding uboLayoutBinding;
    uboLayoutBinding.binding = _bindings.size();
    uboLayoutBinding.descriptorType = vk::DescriptorType::eUniformBuffer;
    uboLayoutBinding.descriptorCount = bufSize;
    uboLayoutBinding.stageFlags = stageFlags;

    _bindings.push_back(uboLayoutBinding);
    increaseDescriptorsCount(vk::DescriptorType::eUniformBuffer, bufSize);

    return *this;
}

DescriptorSetLayoutInfo &DescriptorSetLayoutInfo::addCombinedImageSampler(vk::ShaderStageFlags stageFlags) {
    vk::DescriptorSetLayoutBinding samplerLayoutBinding;
    samplerLayoutBinding.binding = _bindings.size();
    samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.pImmutableSamplers = nullptr; //Optional
    samplerLayoutBinding.stageFlags = stageFlags;

    _bindings.push_back(samplerLayoutBinding);
    increaseDescriptorsCount(vk::DescriptorType::eCombinedImageSampler, 1);

    return *this;
}

DescriptorSetLayoutInfo &DescriptorSetLayoutInfo::addArrayOfCombinedImageSamplers(vk::ShaderStageFlags stageFlags, int count) {
    vk::DescriptorSetLayoutBinding samplerLayoutBinding;
    samplerLayoutBinding.binding = _bindings.size();
    samplerLayoutBinding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    samplerLayoutBinding.descriptorCount = count;
    samplerLayoutBinding.pImmutableSamplers = nullptr; //Optional
    samplerLayoutBinding.stageFlags = stageFlags;

    _bindings.push_back(samplerLayoutBinding);
    increaseDescriptorsCount(vk::DescriptorType::eCombinedImageSampler, count);

    return *this;
}


DescriptorSetLayoutInfo &DescriptorSetLayoutInfo::addStorageImage(vk::ShaderStageFlags stageFlags) {
    vk::DescriptorSetLayoutBinding storageImageLayoutBinding;
    storageImageLayoutBinding.binding = _bindings.size();
    storageImageLayoutBinding.descriptorType = vk::DescriptorType::eStorageImage;
    storageImageLayoutBinding.descriptorCount = 1;
    storageImageLayoutBinding.pImmutableSamplers = nullptr; //Optional
    storageImageLayoutBinding.stageFlags = stageFlags;

    _bindings.push_back(storageImageLayoutBinding);
    increaseDescriptorsCount(vk::DescriptorType::eStorageImage, 1);

    return *this;
}

void DescriptorSetLayoutInfo::increaseDescriptorsCount(vk::DescriptorType descrType, int descrCount) {
    if (_poolSizes.find(descrType) != _poolSizes.end()) {
        _poolSizes[descrType] += descrCount;
    } else {
        _poolSizes[descrType] = descrCount;
    }
}