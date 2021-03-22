#include "magma/vk/pipeline/PipelineInfo.h"

#include "magma/vk/vulkan_common.h"
#include <vulkan/vulkan.hpp>

PipelineLayoutInfo::PipelineLayoutInfo()
        : _pipelineLayoutInfo()
{}

PipelineLayoutInfo::PipelineLayoutInfo(const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts)
        : _pipelineLayoutInfo({}, descriptorSetLayouts)
{}

PipelineLayoutInfo::PipelineLayoutInfo(const vk::DescriptorSetLayout &descriptorSetLayout)
        : _pipelineLayoutInfo({}, 1, &descriptorSetLayout)
{}

PipelineInfo::PipelineInfo(vk::Extent2D extent) 
        : _resolution(extent) {

    //vertex input info
    //use setVertexInputInfo to change
    _vertexInput.vertexBindingDescriptionCount = 0;
    _vertexInput.pVertexBindingDescriptions = nullptr; // Optional
    _vertexInput.vertexAttributeDescriptionCount = 0;
    _vertexInput.pVertexAttributeDescriptions = nullptr; // Optional

    //topology
    _inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
    _inputAssembly.primitiveRestartEnable = false;

    //viewport
    _viewport.x = 0.0f;
    _viewport.y = 0.0f;
    _viewport.width  = (float) extent.width;
    _viewport.height = (float) extent.height;
    _viewport.minDepth = 0.0f;
    _viewport.maxDepth = 1.0f;

    //scissor
    _scissor.offset = vk::Offset2D(0, 0);
    _scissor.extent = extent;

    _viewportState.viewportCount = 1;
    _viewportState.pViewports = &_viewport;
    _viewportState.scissorCount = 1;
    _viewportState.pScissors = &_scissor;

    //rasterizer
    _rasterization.depthClampEnable = false; //to set true requires gpu feature
    _rasterization.rasterizerDiscardEnable = false;
    _rasterization.polygonMode = vk::PolygonMode::eFill; //can be changed to line or point, req gpu feature
    _rasterization.lineWidth = 1.0f; //check wideLines gpu feature
    _rasterization.cullMode = vk::CullModeFlagBits::eNone; //VK_CULL_MODE_BACK_BIT
    _rasterization.frontFace = vk::FrontFace::eClockwise;
    _rasterization.depthBiasEnable = false;
    _rasterization.depthBiasConstantFactor = 0.0f; // Optional
    _rasterization.depthBiasClamp = 0.0f; // Optional
    _rasterization.depthBiasSlopeFactor = 0.0f; // Optional

    //multisampling
    _multisampleState.sampleShadingEnable = false;
    _multisampleState.rasterizationSamples = vk::SampleCountFlagBits::e1;
    _multisampleState.minSampleShading = 1.0f; // Optional
    _multisampleState.pSampleMask = nullptr; // Optional
    _multisampleState.alphaToCoverageEnable = false; // Optional
    _multisampleState.alphaToOneEnable = false; // Optional

    //depth and stencil test
    _depthStencil.depthTestEnable = true;
    _depthStencil.depthWriteEnable = true;
    _depthStencil.depthCompareOp = vk::CompareOp::eLess;
    _depthStencil.depthBoundsTestEnable = false;
    _depthStencil.minDepthBounds = 0.0f; // Optional
    _depthStencil.maxDepthBounds = 1.0f; // Optional
    _depthStencil.stencilTestEnable = false;
    _depthStencil.front = vk::StencilOpState();
    _depthStencil.back = vk::StencilOpState(); // Optional

    //color blending for one framebuffer
    _colorBlendAttachments = { createColorBlendAttachmentState() };

    //full color blending settings
    _colorBlend.logicOpEnable = false;
    _colorBlend.logicOp = vk::LogicOp::eCopy; // Optional
    _colorBlend.attachmentCount = static_cast<uint32_t>(_colorBlendAttachments.size());
    _colorBlend.pAttachments = _colorBlendAttachments.data();
    _colorBlend.blendConstants[0] = 0.0f; // Optional
    _colorBlend.blendConstants[1] = 0.0f; // Optional
    _colorBlend.blendConstants[2] = 0.0f; // Optional
    _colorBlend.blendConstants[3] = 0.0f; // Optional

    //the ability to change viewport
    _dynamicStates = {
            vk::DynamicState::eViewport,
            vk::DynamicState::eLineWidth
    };

    _dynamicState.dynamicStateCount = _dynamicStates.size();
    _dynamicState.pDynamicStates = _dynamicStates.data();

    _layoutInfo.setLayoutCount = 0; // setLayouts
    _layoutInfo.pSetLayouts = nullptr; // setLayouts
    _layoutInfo.pushConstantRangeCount = 0; // Optional
    _layoutInfo.pPushConstantRanges = nullptr; // Optional
}

vk::PipelineColorBlendAttachmentState PipelineInfo::createColorBlendAttachmentState() {
    vk::PipelineColorBlendAttachmentState colorBlendAttachment;
    colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | 
        vk::ColorComponentFlagBits::eG |
        vk::ColorComponentFlagBits::eB |
        vk::ColorComponentFlagBits::eA;
    colorBlendAttachment.blendEnable = false;
    colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd; // Optional
    colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eOne; // Optional
    colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero; // Optional
    colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd; // Optional

    return colorBlendAttachment;
}

void PipelineInfo::setVertexInputInfo(const std::vector<vk::VertexInputBindingDescription> &bindingInfo,
        const std::vector<vk::VertexInputAttributeDescription> &attributeInfo) {
    _vertexInput.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingInfo.size());
    _vertexInput.pVertexBindingDescriptions = bindingInfo.data();
    _vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeInfo.size());
    _vertexInput.pVertexAttributeDescriptions = attributeInfo.data();
}

void PipelineInfo::setLayout(const PipelineLayoutInfo &pipelineLayoutInfo) {
    _layoutInfo = pipelineLayoutInfo.getPipelineLayoutInfo();
}

void PipelineInfo::setDepthCompareOp(vk::CompareOp op) {
    _depthStencil.depthCompareOp = op;
}

void PipelineInfo::setDepthBias(vk::Bool32 depthBiasEnable, float depthBiasConstantFactor) {
    _rasterization.depthBiasEnable = depthBiasEnable;
    _rasterization.depthBiasConstantFactor = depthBiasConstantFactor;
}

void PipelineInfo::setColorBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState> &attachments) {
    _colorBlendAttachments = attachments;
    _colorBlend.attachmentCount = static_cast<uint32_t>(_colorBlendAttachments.size());
    _colorBlend.pAttachments = _colorBlendAttachments.data();
}

