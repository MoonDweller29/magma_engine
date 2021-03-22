#pragma once
#include <vulkan/vulkan.hpp>

#include "magma/vk/pipeline/PipelineInfo.h"

class ComputePipeline {
public:
    ComputePipeline(
            vk::Device device,
            const vk::PipelineShaderStageCreateInfo &shaderStage,
            const PipelineLayoutInfo &pipelineLayoutInfo);
    ~ComputePipeline();

    vk::Pipeline          getPipeline()       const { return _pipeline;       }
    vk::PipelineLayout    getPipelineLayout() const { return _pipelineLayout; }
private:
    vk::Device            _device;
    vk::PipelineLayout    _pipelineLayout;
    vk::Pipeline          _pipeline;
};