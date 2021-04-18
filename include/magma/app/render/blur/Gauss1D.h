#pragma once

#include <memory>

#include "magma/vk/LogicalDevice.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/pipeline/GraphicsPipeline.h"
#include "magma/vk/FrameBuffer.h"
#include "magma/vk/vulkan_common.h"


class Gauss1D {
public:
    Gauss1D(vk::Device device, Texture inpTex, Texture outTex, int axis, Queue queue);
    NON_COPYABLE(Gauss1D);

    const CmdSync &getSync() const { return _renderFinished; }

    const CmdSync &draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);

private:
    vk::Device _device;
    const Queue _queue;
    CommandBuffer _cmdBuf;
    CmdSync _renderFinished;
    const Texture _outTex;
    const int     _axis;
    vk::UniqueSampler _imageSampler;
    vk::Extent2D _extent;

    DescriptorSetLayout  _descriptorSetLayout;
    vk::DescriptorSet    _descriptorSet;
    vk::UniqueRenderPass _renderPass;
    std::unique_ptr<GraphicsPipeline> _graphicsPipeline;

    FrameBuffer _frameBuffer;

    vk::UniqueRenderPass createRenderPass();
    vk::UniqueSampler createImageSampler();
    void writeDescriptorSets(vk::ImageView inpTexView);
    void recordCmdBuffers();
};