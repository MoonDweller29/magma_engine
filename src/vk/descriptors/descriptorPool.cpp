#include "descriptorPool.h"
#include "vk/vulkan_common.h"
#include <vector>

uint32_t DescriptorPool::DEFAULT_SET_COUNT = 10;

DescriptorPool::DescriptorPool(
        VkDevice device,
        const std::array<VkDescriptorPoolSize, VK_DESCRIPTOR_TYPE_RANGE_SIZE> &pool_sizes,
        uint32_t max_set_count
):
    maxSetCount(max_set_count), device(device), setCount(0)
{
    std::vector<VkDescriptorPoolSize> nonzeroPoolSizes;
    for (auto poolSize : pool_sizes)
    {
        if (poolSize.descriptorCount > 0)
        {
            auto actualPoolSize = poolSize;
            actualPoolSize.descriptorCount *= max_set_count;
            nonzeroPoolSizes.push_back(actualPoolSize);
        }
    }

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(nonzeroPoolSizes.size());
    poolInfo.pPoolSizes = nonzeroPoolSizes.data();
    poolInfo.maxSets = max_set_count;
    poolInfo.flags = 0; //Optional

    VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
    vk_check_err(result, "failed to create descriptor pool!");
}

DescriptorPool::DescriptorPool(DescriptorPool &&other_pool):
        device(other_pool.device),
        pool(other_pool.pool),
        maxSetCount(other_pool.maxSetCount),
        setCount(other_pool.setCount)
{
    other_pool.pool = VK_NULL_HANDLE;
}

VkDescriptorSet DescriptorPool::allocateSet(VkDescriptorSetLayout layout)
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet descriptorSet;
    VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &descriptorSet);
    vk_check_err(result, "failed to allocate descriptor sets!");
    setCount+=1;

    return descriptorSet;
}

//can return less descriptors than was required if gets full
std::vector<VkDescriptorSet> DescriptorPool::allocateSets(VkDescriptorSetLayout layout, uint32_t count)
{
    uint32_t descriptorCount = std::min(count, maxSetCount - setCount);
    std::vector<VkDescriptorSet> descriptorSets(descriptorCount);
    if (descriptorCount == 0)
        return descriptorSets;
    std::vector<VkDescriptorSetLayout> layouts(descriptorCount, layout);


    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = descriptorCount;
    allocInfo.pSetLayouts = layouts.data();

    VkResult result = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
    vk_check_err(result, "failed to allocate descriptor sets!");
    setCount += descriptorCount;

    return descriptorSets;
}

DescriptorPool::~DescriptorPool()
{
    vkDestroyDescriptorPool(device, pool, nullptr);
}