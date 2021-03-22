#include "magma/vk/pipeline/GraphicsPipeline.h"

#include "magma/vk/vulkan_common.h"

GraphicsPipeline::GraphicsPipeline(vk::Device device,
    const std::vector<vk::PipelineShaderStageCreateInfo> &shaderStages,
    const PipelineInfo &pipelineInfo, vk::RenderPass renderPass)
        : _device(device) {
    vk::Result result;
    std::tie(result, _pipelineLayout) = _device.createPipelineLayout(pipelineInfo.getLayoutInfo());
    VK_CHECK_ERR(result, "Failed to create pipeline layout!");

    vk::GraphicsPipelineCreateInfo graphicsPipelineInfo;
    graphicsPipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    graphicsPipelineInfo.pStages = shaderStages.data();
    graphicsPipelineInfo.pVertexInputState = &pipelineInfo.getVertexInputInfo();
    graphicsPipelineInfo.pInputAssemblyState = &pipelineInfo.getAssemblyInfo();
    graphicsPipelineInfo.pViewportState = &pipelineInfo.getViewportInfo();
    graphicsPipelineInfo.pRasterizationState = &pipelineInfo.getRasterizationInfo();
    graphicsPipelineInfo.pMultisampleState = &pipelineInfo.getMultisampleInfo();
    graphicsPipelineInfo.pDepthStencilState = &pipelineInfo.getDepthStencilInfo();
    graphicsPipelineInfo.pColorBlendState = &pipelineInfo.getColorBlendInfo();
    graphicsPipelineInfo.layout = _pipelineLayout;
    graphicsPipelineInfo.renderPass = renderPass;

    std::tie(result, _graphicsPipeline) = _device.createGraphicsPipeline({}, graphicsPipelineInfo);
    VK_CHECK_ERR(result, "failed to create graphics pipeline!");
}

GraphicsPipeline::~GraphicsPipeline() {
    _device.destroyPipeline(_graphicsPipeline);
    _device.destroyPipelineLayout(_pipelineLayout);
}