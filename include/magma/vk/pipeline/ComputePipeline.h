#pragma once
#include <vulkan/vulkan.hpp>

#include "magma/vk/pipeline/PipelineInfo.h"
#include "magma/vk/vulkan_common.h"

class ComputePipeline {
public:
    ComputePipeline(
            vk::Device device,
            const vk::PipelineShaderStageCreateInfo &shaderStage,
            const PipelineLayoutInfo &pipelineLayoutInfo);
    NON_COPYABLE(ComputePipeline);
    ~ComputePipeline();

    vk::Pipeline          getPipeline()       const { return _pipeline;       }
    vk::PipelineLayout    getPipelineLayout() const { return _pipelineLayout; }
private:
    vk::Device            _device;
    vk::PipelineLayout    _pipelineLayout;
    vk::Pipeline          _pipeline;
};