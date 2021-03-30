#include "magma/vk/descriptors/DescriptorSetLayout.h"

#include <sstream>
#include "magma/vk/vulkan_common.h"

DescriptorSetLayout::DescriptorSetLayout(vk::Device device, const DescriptorSetLayoutInfo &layoutInfo) :
    _device(device), _layoutInfo(layoutInfo), _setInd(0)
{
    vk::DescriptorSetLayoutCreateInfo layoutCreateInfo({}, _layoutInfo.getBindings());

    vk::Result result;
    std::tie(result, _layout) = _device.createDescriptorSetLayout(layoutCreateInfo);
    VK_CHECK_ERR(result, "failed to create descriptor set layout!");

    _pools.emplace_back(device, _layoutInfo.getPoolSizes(), DescriptorPool::DEFAULT_SET_COUNT);
}

DescriptorSetLayout::~DescriptorSetLayout() {
    _pools.clear();
    _device.destroyDescriptorSetLayout(_layout);
}

void DescriptorSetLayout::freePool() {
    _pools.clear();
    _pools.emplace_back(_device, _layoutInfo.getPoolSizes(), DescriptorPool::DEFAULT_SET_COUNT);
}

void DescriptorSetLayout::allocateSets(uint32_t count) {
    _descriptorSetInfo.clear();
    _descriptorSets.clear();
    //iteration over existing pools
    for (auto &pool : _pools) {
        if (pool.isFull()) {
            continue;
        }
        auto newSets = pool.allocateSets(_layout, count);
        count -= newSets.size();
        _descriptorSets.insert(_descriptorSets.end(), newSets.begin(), newSets.end());
    }
    //allocation of new pools if needed
    while (count > 0) {
        _pools.emplace_back(_device, _layoutInfo.getPoolSizes(), DescriptorPool::DEFAULT_SET_COUNT);
        auto newSets = _pools.back().allocateSets(_layout, count);
        count -= newSets.size();
        _descriptorSets.insert(_descriptorSets.end(), newSets.begin(), newSets.end());
    }
}

void DescriptorSetLayout::beginSet(uint32_t ind) {
    if (ind > _descriptorSets.size()) {
        LOG_AND_THROW std::runtime_error("beginSet: wrong descriptorSet index");
    }
    _setInd = ind;
}

void DescriptorSetLayout::bindUniformBuffer(
        uint32_t binding, vk::Buffer buf, vk::DeviceSize offset, vk::DeviceSize range
) {
    checkBindingIndex(binding, vk::DescriptorType::eUniformBuffer);

    vk::DescriptorBufferInfo *bufferInfo = _descriptorSetInfo.newBufferInfo();
    bufferInfo->buffer = buf;
    bufferInfo->offset = offset;
    bufferInfo->range = range; //size of all buffer (not the size of one elem)

    vk::WriteDescriptorSet &descriptorWrite = _descriptorSetInfo.newDescriptorWriteInfo();
    descriptorWrite.dstSet = _descriptorSets[_setInd];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0; //first index in array
    descriptorWrite.descriptorType = vk::DescriptorType::eUniformBuffer;
    descriptorWrite.descriptorCount = _layoutInfo.getBindings()[binding].descriptorCount;
    descriptorWrite.pBufferInfo = bufferInfo;
}

void DescriptorSetLayout::bindCombinedImageSampler(uint32_t binding, vk::ImageView imageView, vk::Sampler sampler) {
    checkBindingIndex(binding, vk::DescriptorType::eCombinedImageSampler);

    vk::DescriptorImageInfo *imageInfo = _descriptorSetInfo.newImageInfo();
    imageInfo->imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
    imageInfo->imageView = imageView;
    imageInfo->sampler = sampler;

    vk::WriteDescriptorSet &descriptorWrite = _descriptorSetInfo.newDescriptorWriteInfo();
    descriptorWrite.dstSet = _descriptorSets[_setInd];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eCombinedImageSampler;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = imageInfo;
}

void DescriptorSetLayout::bindStorageImage(uint32_t binding, vk::ImageView imageView, vk::ImageLayout imageLayout) {
    checkBindingIndex(binding, vk::DescriptorType::eStorageImage);

    vk::DescriptorImageInfo *imageInfo = _descriptorSetInfo.newImageInfo();
    imageInfo->imageLayout = imageLayout;
    imageInfo->imageView = imageView;
    imageInfo->sampler = vk::Sampler();

    vk::WriteDescriptorSet &descriptorWrite = _descriptorSetInfo.newDescriptorWriteInfo();
    descriptorWrite.dstSet = _descriptorSets[_setInd];
    descriptorWrite.dstBinding = binding;
    descriptorWrite.dstArrayElement = 0;
    descriptorWrite.descriptorType = vk::DescriptorType::eStorageImage;
    descriptorWrite.descriptorCount = 1;
    descriptorWrite.pImageInfo = imageInfo;
}

std::vector<vk::DescriptorSet> DescriptorSetLayout::recordAndReturnSets() {
    const auto &descriptorWrites = _descriptorSetInfo.getDescriptorWrites();
    _device.updateDescriptorSets(descriptorWrites,nullptr);

    return _descriptorSets;
}

void DescriptorSetLayout::checkBindingIndex(uint32_t bindingInd, vk::DescriptorType descriptorType) {
    if (bindingInd >= _layoutInfo.getBindings().size()) {
        LOG_AND_THROW std::out_of_range("binding index <"+std::to_string(bindingInd)+"> is out of range");
    }
    if (_layoutInfo.getBindings()[bindingInd].descriptorType != descriptorType) {
        std::stringstream message;
        message << "binding index mismatch:\n binding <" << bindingInd <<
                "> has type <" << to_string(_layoutInfo.getBindings()[bindingInd].descriptorType) <<
                "> which is not <" << to_string(descriptorType) << ">";
        LOG_AND_THROW std::runtime_error(message.str());
    }
}
