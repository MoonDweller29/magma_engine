#include "magma/vk/pipeline/ComputePipeline.h"

#include "magma/vk/vulkan_common.h"

ComputePipeline::ComputePipeline(vk::Device device, const vk::PipelineShaderStageCreateInfo &shaderStage,
    const PipelineLayoutInfo &pipelineLayoutInfo)
        : _device(device) {
    
    vk::Result result;
    std::tie(result, _pipelineLayout) = _device.createPipelineLayout(pipelineLayoutInfo.getInfo());
    VK_CHECK_ERR(result, "Failed to create compute pipeline layout!");

    vk::ComputePipelineCreateInfo computePipelineCreateInfo({}, shaderStage, _pipelineLayout);
    std::tie(result, _pipeline) = _device.createComputePipeline({}, computePipelineCreateInfo);
    VK_CHECK_ERR(result, "Failed to create compute pipeline!");
}

ComputePipeline::~ComputePipeline() {
    _device.destroyPipeline(_pipeline);
    _device.destroyPipelineLayout(_pipelineLayout);
}