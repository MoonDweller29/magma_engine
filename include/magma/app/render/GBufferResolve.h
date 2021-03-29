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

class GBufferResolve {
public:
    GBufferResolve(vk::Device device, Texture renderTarget, Queue queue);
    ~GBufferResolve() = default;

    const CmdSync &getSync() const { return _renderFinished; }

    void writeDescriptorSets(
            const GBuffer &gBuffer,
            vk::ImageView shadowMapView, vk::Sampler shadowMapSampler,
            const Buffer &fragmentUniform, uint32_t fuboSize,
            const Buffer &lightSpaceUniform, uint32_t luboSize);
    void recordCmdBuffers();
    const CmdSync &draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);

private:
    vk::Device _device;
    const Queue _queue;
    CommandBuffer _cmdBuf;
    CmdSync _renderFinished;
    const Texture _renderTarget;
    vk::UniqueSampler _imgSampler;
    vk::Extent2D _extent;

    DescriptorSetLayout  _descriptorSetLayout;
    vk::DescriptorSet    _descriptorSet;
    vk::UniqueRenderPass _renderPass;
    std::unique_ptr<GraphicsPipeline> _graphicsPipeline;

    FrameBuffer _frameBuffer;

    void initDescriptorSetLayout();
    vk::UniqueRenderPass createRenderPass();
    vk::UniqueSampler createImageSampler();
};