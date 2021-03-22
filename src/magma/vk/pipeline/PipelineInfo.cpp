#include "magma/vk/pipeline/PipelineInfo.h"

#include "magma/vk/vulkan_common.h"

PipelineLayoutInfo::PipelineLayoutInfo()
        : _pipelineLayoutInfo()
{}

PipelineLayoutInfo::PipelineLayoutInfo(const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts)
        : _descriptorSetLayouts(descriptorSetLayouts),
          _pipelineLayoutInfo({}, _descriptorSetLayouts)
{}

PipelineLayoutInfo::PipelineLayoutInfo(const vk::DescriptorSetLayout &descriptorSetLayout)
        : _descriptorSetLayouts(1, descriptorSetLayout),
        _pipelineLayoutInfo({}, _descriptorSetLayouts)
{}

PipelineVertexInputInfo::PipelineVertexInputInfo(const std::vector<vk::VertexInputBindingDescription> &bindingInfo,
        const std::vector<vk::VertexInputAttributeDescription> &attributeInfo)
        : _bindingInfo(bindingInfo),
        _attributeInfo(attributeInfo),
        _pipelineVertexInputInfo({}, _bindingInfo, _attributeInfo)
{}

PipelineInfo::PipelineInfo(vk::Extent2D extent) 
        : _resolution(extent) {

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
    _rasterizer.depthClampEnable = false; //to set true requires gpu feature
    _rasterizer.rasterizerDiscardEnable = false;
    _rasterizer.polygonMode = vk::PolygonMode::eFill; //can be changed to line or point, req gpu feature
    _rasterizer.lineWidth = 1.0f; //check wideLines gpu feature
    _rasterizer.cullMode = vk::CullModeFlagBits::eNone; //VK_CULL_MODE_BACK_BIT
    _rasterizer.frontFace = vk::FrontFace::eClockwise;
    _rasterizer.depthBiasEnable = false;
    _rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    _rasterizer.depthBiasClamp = 0.0f; // Optional
    _rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

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

void PipelineInfo::setVertexInputInfo(const PipelineVertexInputInfo &pipelineVertexInputInfo) {
    _vertexInfo = pipelineVertexInputInfo;
}

void PipelineInfo::setLayout(const PipelineLayoutInfo &pipelineLayoutInfo) {
    _layoutInfo = pipelineLayoutInfo;
}

void PipelineInfo::setDepthCompareOp(vk::CompareOp op) {
    _depthStencil.depthCompareOp = op;
}

void PipelineInfo::setDepthBias(vk::Bool32 depthBiasEnable, float depthBiasConstantFactor) {
    _rasterizer.depthBiasEnable = depthBiasEnable;
    _rasterizer.depthBiasConstantFactor = depthBiasConstantFactor;
}

void PipelineInfo::setColorBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState> &attachments) {
    _colorBlendAttachments = attachments;
    _colorBlend.attachmentCount = static_cast<uint32_t>(_colorBlendAttachments.size());
    _colorBlend.pAttachments = _colorBlendAttachments.data();
}

