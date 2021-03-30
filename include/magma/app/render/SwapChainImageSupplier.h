#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>
#include <vector>

#include "magma/vk/SwapChain.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/commands/CommandBufferArr.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/pipeline/GraphicsPipeline.h"
#include "magma/vk/FrameBuffer.h"


class SwapChainImageSupplier {
public:
    SwapChainImageSupplier(vk::Device device, vk::ImageView inputImageView, SwapChain &swapChain, Queue queue);
    ~SwapChainImageSupplier() = default;

    const CmdSync &getSync() const { return _renderFinished; }

    void recordCmdBuffers();
    const CmdSync &draw(
            int imgInd,
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);


private:
    vk::Device _device;
    SwapChain &_swapChain;
    const Queue _queue;
    CommandBufferArr _cmdBufArr;
    CmdSync _renderFinished;
    vk::UniqueSampler _imgSampler;
    vk::Extent2D _extent;

    DescriptorSetLayout  _descriptorSetLayout;
    vk::DescriptorSet    _descriptorSet;
    vk::UniqueRenderPass _renderPass;
    std::unique_ptr<GraphicsPipeline> _graphicsPipeline;

    std::vector<FrameBuffer> _frameBuffers;

    void writeDescriptorSets(vk::ImageView inputImageView);
    vk::UniqueRenderPass createRenderPass();
    vk::UniqueSampler createImageSampler();
};