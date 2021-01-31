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

CmdSync ComputePass::compute() {
    _cmdSync.resetFence();
    vk::SubmitInfo computeSubmitInfo;
    computeSubmitInfo.commandBufferCount = 1;
    computeSubmitInfo.pCommandBuffers = &_commandBuffer.getCmdBuf();
    computeSubmitInfo.waitSemaphoreCount = 0;
    computeSubmitInfo.pWaitSemaphores = nullptr;
    computeSubmitInfo.signalSemaphoreCount = 0;
    computeSubmitInfo.pSignalSemaphores = nullptr;
    vk::Result result = _device.getGraphicsQueue().queue.submit(computeSubmitInfo, _cmdSync.getFence());

    return _cmdSync;
}