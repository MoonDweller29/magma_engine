#pragma once
#include <vulkan/vulkan.h>
#include "vk/graphicsPipeline.h"
#include "vk/logicalDevice.h"
#include "vk/swapChain.h"
#include "vk/cmdSync.h"
#include "vk/commandBuffer.h"
#include "vk/descriptors/descriptorSetLayout.h"
#include "app/scene/mesh.h"
#include <memory>

class ColorPass
{
    LogicalDevice &device;
    SwapChain &swapChain;

    DescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;
    VkRenderPass renderPass;
    std::unique_ptr<GraphicsPipeline> graphicsPipeline;
    CommandBufferArr commandBuffers;

    const VkExtent2D extent;
    CmdSync renderFinished;

    void createRenderPass();
    void initDescriptorSetLayout();
public:
    ColorPass(LogicalDevice &device, SwapChain &swapChain);
    void writeDescriptorSets(
            const Buffer &uniformBuffer, uint32_t ubo_size,
            const Buffer &fragmentUniform, uint32_t fu_size,
            VkImageView tex_view, VkSampler sampler,
            const Buffer &lightSpaceUniform, uint32_t lu_size,
            VkImageView shadow_map_view, VkSampler shadow_sampler);
    VkRenderPass getRenderPass() const { return renderPass; }
    void recordCmdBuffers(
            VkBuffer indexBuffer,
            VkBuffer vertexBuffer,
            uint32_t vertexCount,
            const std::vector<VkFramebuffer> &frameBuffers);

    CmdSync draw(uint32_t i,
                 const std::vector<VkSemaphore> &waitSemaphores,
                 const std::vector<VkFence> &waitFences);
    const CmdSync &getSync() { return renderFinished; }
    ~ColorPass();

};