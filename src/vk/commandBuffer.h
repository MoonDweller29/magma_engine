#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class GraphicsPipeline;

class CommandBufferArr
{
    std::vector<VkCommandBuffer> commandBuffers;
public:
    CommandBufferArr() = default;
    CommandBufferArr(VkDevice device, VkCommandPool commandPool, uint32_t count);
    void allocate(VkDevice device, VkCommandPool commandPool, uint32_t count);
    void record(
            VkBuffer indexBuffer,
            VkBuffer vertexBuffer,
            uint32_t vertexCount,
            const std::vector<VkDescriptorSet> &descriptorSets,
            VkRenderPass renderPass,
            VkExtent2D extent,
            const std::vector<VkFramebuffer> &frameBuffers,
            const GraphicsPipeline &graphicsPipeline);
    ~CommandBufferArr();

    const uint32_t size() const { return commandBuffers.size(); }
    const VkCommandBuffer *data() const { return commandBuffers.data(); }
    const VkCommandBuffer &operator[] (const size_t index);
};

class SingleTimeCommandBuffer
{
    VkCommandBuffer commandBuffer;

    VkDevice device;
    VkCommandPool commandPool;
    VkQueue queue;

    VkCommandBufferBeginInfo beginInfo;
public:
    SingleTimeCommandBuffer(VkDevice device, VkCommandPool commandPool, VkQueue queue);

    const VkCommandBuffer &startRecording();
    void endRecordingAndSubmit();

    ~SingleTimeCommandBuffer();
};