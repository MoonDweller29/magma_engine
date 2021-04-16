#pragma once

#include <memory>

#include "magma/vk/LogicalDevice.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/pipeline/GraphicsPipeline.h"
#include "magma/vk/FrameBuffer.h"
#include "magma/vk/vulkan_common.h"

class GBuffer;
class TexPyramid;

class PyramidSSAO {
public:
    PyramidSSAO(vk::Device device, Texture ssaoTex, int pyramidSize, Queue queue);
    NON_COPYABLE(PyramidSSAO);

    const CmdSync &getSync() const { return _renderFinished; }

    void writeDescriptorSets(
            const GBuffer &gBuffer,
            vk::ImageView noiseTexView,
            const TexPyramid &depthPyramid,
            const Buffer &uniformBuffer, uint32_t uboSize,
            const Buffer &inverseProjUniform, uint32_t invProjUboSize);
    void recordCmdBuffers();
    const CmdSync &draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);

private:
    vk::Device _device;
    const Queue _queue;
    CommandBuffer _cmdBuf;
    CmdSync _renderFinished;
    const Texture _ssaoTex;
    vk::UniqueSampler _depthNormalsSampler;
    vk::UniqueSampler _noiseSampler;
    vk::UniqueSampler _depthPyramidSampler;
    vk::Extent2D _extent;

    DescriptorSetLayout  _descriptorSetLayout;
    vk::DescriptorSet    _descriptorSet;
    vk::UniqueRenderPass _renderPass;
    std::unique_ptr<GraphicsPipeline> _graphicsPipeline;

    FrameBuffer _frameBuffer;

    vk::UniqueRenderPass createRenderPass();
    vk::UniqueSampler createDepthNormalsSampler();
    vk::UniqueSampler createNoiseSampler();
    vk::UniqueSampler createDepthPyramidSampler();
};