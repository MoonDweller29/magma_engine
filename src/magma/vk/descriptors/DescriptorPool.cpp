#include "magma/vk/descriptors/DescriptorPool.h"

#include <algorithm>

#include "magma/vk/vulkan_common.h"

uint32_t DescriptorPool::DEFAULT_SET_COUNT = 10;

DescriptorPool::DescriptorPool(
        vk::Device device,
        const std::unordered_map<vk::DescriptorType, uint32_t> &poolSizes,
        uint32_t maxSetCount
) :
    _maxSetCount(maxSetCount), _device(device), _setCount(0)
{
    std::vector<vk::DescriptorPoolSize> nonzeroPoolSizes;
    for (auto [descType, descCount] : poolSizes) {
        vk::DescriptorPoolSize actualPoolSize(descType, descCount*maxSetCount);
        nonzeroPoolSizes.push_back(actualPoolSize);
    }

    vk::DescriptorPoolCreateInfo poolInfo;
    poolInfo.poolSizeCount = static_cast<uint32_t>(nonzeroPoolSizes.size());
    poolInfo.pPoolSizes = nonzeroPoolSizes.data();
    poolInfo.maxSets = maxSetCount;
    poolInfo.flags = vk::DescriptorPoolCreateFlags(); //Optional

    vk::Result result;
    std::tie(result, _pool) = _device.createDescriptorPool(poolInfo);
    VK_CHECK_ERR(result, "failed to create descriptor pool!");
}

DescriptorPool::DescriptorPool(DescriptorPool &&otherPool) :
        _device(otherPool._device),
        _pool(otherPool._pool),
        _maxSetCount(otherPool._maxSetCount),
        _setCount(otherPool._setCount)
{
    otherPool._device = vk::Device();
}

DescriptorPool::~DescriptorPool() {
    if (_device) {
        _device.destroyDescriptorPool(_pool);
    }
}

vk::DescriptorSet DescriptorPool::allocateSet(vk::DescriptorSetLayout layout) {
    vk::DescriptorSetAllocateInfo allocInfo;
    allocInfo.descriptorPool = _pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    auto[result, descriptorSets] = _device.allocateDescriptorSets(allocInfo);
    VK_CHECK_ERR(result, "failed to allocate descriptor sets!");
    _setCount+=1;

    return descriptorSets[0];
}

//can return less descriptors than was required if pool gets full
std::vector<vk::DescriptorSet> DescriptorPool::allocateSets(vk::DescriptorSetLayout layout, uint32_t count) {
    uint32_t descriptorSetCount = std::min(count, _maxSetCount - _setCount);
    if (descriptorSetCount == 0) {
        return std::vector<vk::DescriptorSet>();
    }
    std::vector<vk::DescriptorSetLayout> layouts(descriptorSetCount, layout);

    vk::DescriptorSetAllocateInfo allocInfo(_pool, layouts);

    auto[result, descriptorSets] = _device.allocateDescriptorSets(allocInfo);
    VK_CHECK_ERR(result, "failed to allocate descriptor sets!");
    _setCount += descriptorSetCount;

    return descriptorSets;
}

