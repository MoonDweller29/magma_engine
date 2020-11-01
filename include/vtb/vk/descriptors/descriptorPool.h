#pragma once
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <vector>

class DescriptorPool
{
    VkDevice device;
    VkDescriptorPool pool;

    uint32_t setCount;
    uint32_t maxSetCount;
public:
    DescriptorPool(
            VkDevice device,
            const std::unordered_map<VkDescriptorType, uint32_t> &pool_sizes,
            uint32_t max_set_count);
    DescriptorPool(const DescriptorPool &other_pool) = delete;
    DescriptorPool(DescriptorPool &&other_pool);
    ~DescriptorPool();

    bool isFull() const { return setCount == maxSetCount; }
    VkDescriptorSet allocateSet(VkDescriptorSetLayout layout);
    //can return less descriptors than was required if gets full
    std::vector<VkDescriptorSet> allocateSets(VkDescriptorSetLayout layout, uint32_t count);

    static uint32_t DEFAULT_SET_COUNT;
};
