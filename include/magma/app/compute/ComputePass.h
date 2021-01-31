#pragma once
#include <vulkan/vulkan.hpp>

#include "magma/vk/CmdSync.h"
#include "magma/vk/ShaderModule.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/pipeline/ComputePipeline.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"

class ComputePass {
public:
    ComputePass(LogicalDevice &device, DescriptorSetLayoutInfo &descriptorSetLayoutInfo, Shader &computeShader);
    ~ComputePass() = default;

    vk::Pipeline            getPipeline() { return _pipeline.getPipeline(); }
    vk::PipelineLayout      getPipelineLayout() { return _pipeline.getPipelineLayout(); }
    DescriptorSetLayout   & getDecriptorSetLayaut() { return _descriptorSetLayout; }
    CommandBuffer         & getCommandBuffer() { return _commandBuffer; }
    CmdSync               & getCmdSync() { return _cmdSync; }

    CmdSync                 compute();

private:
    LogicalDevice &_device;

    DescriptorSetLayoutInfo _descriptorSetLayoutInfo;
    DescriptorSetLayout     _descriptorSetLayout;
    PipelineLayoutInfo      _pipelineLayoutInfo;
    ComputePipeline         _pipeline;
    CommandBuffer           _commandBuffer;
    CmdSync                 _cmdSync;
};