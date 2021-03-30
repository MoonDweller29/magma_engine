#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>

#include "magma/vk/LogicalDevice.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/pipeline/GraphicsPipeline.h"
#include "magma/vk/FrameBuffer.h"


class GBuffer;

class MainColorPass {
public:
    MainColorPass(vk::Device device, const GBuffer &gBuffer, Queue queue);
    ~MainColorPass() = default;

    const CmdSync &getSync() const { return _renderFinished; }

    void writeDescriptorSets(
            const Buffer &uniformBuffer, uint32_t uboSize,
            vk::ImageView albedoTexView, vk::Sampler sampler);
    void recordCmdBuffers(vk::Buffer indexBuffer, vk::Buffer vertexBuffer, uint32_t vertexCount);
    const CmdSync &draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);

private:
    vk::Device _device;
    const GBuffer &_gBuffer;
    const Queue _queue;
    CommandBuffer _cmdBuf;
    CmdSync _renderFinished;

    DescriptorSetLayout  _descriptorSetLayout;
    vk::DescriptorSet    _descriptorSet;
    vk::UniqueRenderPass _renderPass;
    std::unique_ptr<GraphicsPipeline> _graphicsPipeline;

    FrameBuffer _frameBuffer;


    DescriptorSetLayoutInfo createDescriptorSetLayoutInfo();
    vk::UniqueRenderPass createRenderPass();
};