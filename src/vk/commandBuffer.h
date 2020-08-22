#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class CommandBufferArr
{
    std::vector<VkCommandBuffer> commandBuffers;
public:
    CommandBufferArr() = default;
    CommandBufferArr(VkDevice device, VkCommandPool commandPool, uint32_t count);
    void allocate(VkDevice device, VkCommandPool commandPool, uint32_t count);
    void record(
            VkBuffer vertexBuffer,
            uint32_t vertexCount,
            VkRenderPass renderPass,
            VkExtent2D extent,
            const std::vector<VkFramebuffer> &frameBuffers,
            VkPipeline graphicsPipeline);
    ~CommandBufferArr();

    const uint32_t size() const { return commandBuffers.size(); }
    const VkCommandBuffer *data() const { return commandBuffers.data(); }
    const VkCommandBuffer &operator[] (const size_t index);
};