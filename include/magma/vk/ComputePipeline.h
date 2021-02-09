/**
 * @file ComputePipeline.h
 * @author Nastormo
 * @brief Class create compute pipeline
 * @version 0.1
 * @date 2021-02-09
 */
#pragma once
#include <vulkan/vulkan.h>

#include "magma/vk/graphicsPipeline.h"
#include "shaderModule.h"

class ComputePipelineInfo {
public:
    ComputePipelineInfo();
    ~ComputePipelineInfo() = default;

    const VkPipelineLayoutCreateInfo &getPipelineLayoutInfo() const { return _pipelineLayoutInfo; }

    void setLayout(const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts);
    void setLayout(const VkDescriptorSetLayout &descriptorSetLayout);
private:
    VkPipelineLayoutCreateInfo _pipelineLayoutInfo;
};

class ComputePipeline {
public:
    ComputePipeline(
            VkDevice device,
            const VkPipelineShaderStageCreateInfo &shaderStage,
            const PipelineInfo &pipelineInfo);
    ~ComputePipeline();

    VkPipeline          getPipeline()       const { return _pipeline;       }
    VkPipelineLayout    getPipelineLayout() const { return _pipelineLayout; }
private:
    VkDevice            _device;
    VkPipelineLayout    _pipelineLayout;
    VkPipeline          _pipeline;

};
