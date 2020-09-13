#pragma once
#include <vulkan/vulkan.h>
#include "vk/graphicsPipeline.h"
#include "vk/logicalDevice.h"
#include "vk/cmdSync.h"
#include "vk/commandBuffer.h"
#include "vk/texture.h"
#include "vk/descriptors/descriptorSetLayout.h"
#include "vk/frameBuffer.h"
#include "app/scene/mesh.h"
#include <memory>

class DepthPass
{
    LogicalDevice &device;
    const Texture &depthTex;
    std::unique_ptr<FrameBuffer> frameBuffer;
    VkImageLayout depthFinalLayout;

    DescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet descriptorSet;
    VkRenderPass renderPass;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    CommandBufferArr commandBuffers;

    const VkExtent2D extent;
    CmdSync renderFinished;

    void createRenderPass();
    void initDescriptorSetLayout();
public:
    DepthPass(LogicalDevice &device, const Texture &depthTex, VkExtent2D extent, VkImageLayout depthFinalLayout);
    void writeDescriptorSets(const Buffer &uniformBuffer, uint32_t ubo_size);
    VkRenderPass getRenderPass() const { return renderPass; }
    void recordCmdBuffers(
            VkBuffer indexBuffer,
            VkBuffer vertexBuffer,
            uint32_t vertexCount);

    CmdSync draw(
            const std::vector<VkSemaphore> &waitSemaphores,
            const std::vector<VkFence> &waitFences);
    const CmdSync &getSync() { return renderFinished; }
    ~DepthPass();
};