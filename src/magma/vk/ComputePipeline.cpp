/**
 * @file ComputePipeline.cpp
 * @brief Class create compute pipeline
 * @version 0.1
 * @date 2021-02-09
 */
#include "magma/vk/ComputePipeline.h"

#include "magma/vk/vulkan_common.h"

ComputePipelineInfo::ComputePipelineInfo() {
    _pipelineLayoutInfo = {};
    _pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    _pipelineLayoutInfo.setLayoutCount = 0;
    _pipelineLayoutInfo.pSetLayouts = nullptr;
    _pipelineLayoutInfo.pushConstantRangeCount = 0;
    _pipelineLayoutInfo.pPushConstantRanges = nullptr;
}

void ComputePipelineInfo::setLayout(const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts) {
    _pipelineLayoutInfo.setLayoutCount = descriptorSetLayouts.size();
    _pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
}

void ComputePipelineInfo::setLayout(const VkDescriptorSetLayout &descriptorSetLayout) {
    _pipelineLayoutInfo.setLayoutCount = 1;
    _pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
}

ComputePipeline::ComputePipeline(
        VkDevice device,
        const VkPipelineShaderStageCreateInfo &shaderStage,
        const ComputePipelineInfo &pipelineInfo
) : _device(device) {
    VkResult result;
    result = vkCreatePipelineLayout(device, &pipelineInfo.getPipelineLayoutInfo(), nullptr, &_pipelineLayout);
    VK_CHECK_ERR(result, "failed to create compute pipeline layout!");

    VkComputePipelineCreateInfo computePipelineCreateInfo{};
    computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    computePipelineCreateInfo.flags = 0;
    computePipelineCreateInfo.stage = shaderStage;
    computePipelineCreateInfo.layout = _pipelineLayout;
    result = vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &_pipeline);
    VK_CHECK_ERR(result, "failed to create compute pipeline!");
}

ComputePipeline::~ComputePipeline()
{
    vkDestroyPipeline(_device, _pipeline, nullptr);
    vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
}