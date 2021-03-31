#pragma once
#include <vulkan/vulkan.h>
#include <memory>

#include "magma/vk/pipeline/GraphicsPipeline.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/textures/Texture.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/FrameBuffer.h"
#include "magma/vk/vulkan_common.h"


class DepthPass {
public:
    DepthPass(vk::Device device, const Texture &depthTex, vk::ImageLayout depthFinalLayout, Queue queue);
    NON_COPYABLE(DepthPass);
    void writeDescriptorSets(const Buffer &uniformBuffer, uint32_t uboSize);
    void recordCmdBuffers(vk::Buffer indexBuffer, vk::Buffer vertexBuffer, uint32_t vertexCount);

    CmdSync draw(
            const std::vector<vk::Semaphore> &waitSemaphores,
            const std::vector<vk::Fence> &waitFences);
    const CmdSync &getSync() { return _renderFinished; }

private:
    vk::Device _device;
    const Queue _queue;
    const Texture &_depthTex;
    vk::ImageLayout _depthFinalLayout;
    std::unique_ptr<FrameBuffer> _frameBuffer;
    const vk::Extent2D _extent;

    DescriptorSetLayout _descriptorSetLayout;
    vk::DescriptorSet _descriptorSet;
    vk::UniqueRenderPass _renderPass;
    std::unique_ptr<GraphicsPipeline> _graphicsPipeline;
    CommandBuffer _cmdBuf;

    CmdSync _renderFinished;

    vk::UniqueRenderPass createRenderPass();
};
