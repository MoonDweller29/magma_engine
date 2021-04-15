#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>

#include "magma/vk/LogicalDevice.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/pipeline/GraphicsPipeline.h"
#include "magma/vk/FrameBuffer.h"
#include "magma/vk/vulkan_common.h"

class DepthGather {
public:
    DepthGather(vk::Device device, Texture inputTex, Texture outputTex, Queue queue, bool isFirstStep);
    NON_COPYABLE(DepthGather);

    const CmdSync &getSync() const { return _renderFinished; }

    const CmdSync &draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);

private:
    const int _scale;
    vk::Device _device;
    const Queue _queue;
    CommandBuffer _cmdBuf;
    CmdSync _renderFinished;
    const Texture _inputTex;
    const Texture _outputTex;
    vk::UniqueSampler _imgSampler;
    vk::Extent2D _extent;

    DescriptorSetLayout  _descriptorSetLayout;
    vk::DescriptorSet    _descriptorSet;
    vk::UniqueRenderPass _renderPass;
    std::unique_ptr<GraphicsPipeline> _graphicsPipeline;

    FrameBuffer _frameBuffer;

    vk::UniqueRenderPass createRenderPass();
    vk::UniqueSampler createImageSampler();
    void writeDescriptorSets();
    void recordCmdBuffers();
};