#include "magma/app/compute/ComputePass.h"

#include <vulkan/vulkan.hpp>

ComputePass::ComputePass(LogicalDevice &device,
    DescriptorSetLayoutInfo &descriptorSetLayoutInfo, Shader &computeShader)
        : _device(device),
        _descriptorSetLayoutInfo(descriptorSetLayoutInfo),
        _descriptorSetLayout(_device.getDevice(), _descriptorSetLayoutInfo),
        _pipelineLayoutInfo(_descriptorSetLayout.getLayout()),
        _pipeline(_device.getDevice(), computeShader.getStageInfo(), _pipelineLayoutInfo),
        _commandBuffer(_device.getDevice(), _device.getGraphicsQueue().cmdPool),
        _cmdSync(_device.getDevice())
{}

CmdSync ComputePass::compute(
        const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences
) {
    vk::Result result;
    if (!waitFences.empty()) {
        result = _device.getDevice().waitForFences(waitFences, VK_TRUE, UINT64_MAX);
    }

    vk::SubmitInfo submitInfo;

    std::vector<vk::PipelineStageFlags> waitStages(waitSemaphores.size(), vk::PipelineStageFlagBits::eColorAttachmentOutput);
    if (waitSemaphores.empty()) {
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
    } else if (std::find(waitSemaphores.begin(), waitSemaphores.end(), _cmdSync.getSemaphore()) == waitSemaphores.end()) {
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.data();

        submitInfo.pWaitDstStageMask = waitStages.data();
    } else {
        LOG_AND_THROW std::logic_error("cyclic dependency in render graph");
    }

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_commandBuffer.getCmdBuf();

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &_cmdSync.getSemaphore();

    _cmdSync.resetFence();

    result = _device.getGraphicsQueue().queue.submit(submitInfo, _cmdSync.getFence());
    VK_CHECK_ERR(result, "failed to submit draw command buffer!");

    return _cmdSync;
}