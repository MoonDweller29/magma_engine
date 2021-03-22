#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

class PipelineLayoutInfo {
public:
    PipelineLayoutInfo();
    PipelineLayoutInfo(const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts);
    PipelineLayoutInfo(const vk::DescriptorSetLayout &descriptorSetLayout);

    ~PipelineLayoutInfo() = default;

    const vk::PipelineLayoutCreateInfo &getPipelineLayoutInfo() const { return _pipelineLayoutInfo; }

private:
    vk::PipelineLayoutCreateInfo _pipelineLayoutInfo;
};


class PipelineInfo {
public:
    PipelineInfo(vk::Extent2D extent);
    ~PipelineInfo() = default;

    static vk::PipelineColorBlendAttachmentState createColorBlendAttachmentState();

    const vk::PipelineColorBlendStateCreateInfo     &getColorBlendInfo()    const { return _colorBlend; }
    const vk::PipelineDepthStencilStateCreateInfo   &getDepthStencilInfo()  const { return _depthStencil; }
    const vk::PipelineDynamicStateCreateInfo        &getDynamicStateInfo()  const { return _dynamicState; }
    const vk::PipelineInputAssemblyStateCreateInfo  &getAssemblyInfo()      const { return _inputAssembly; }
    const vk::PipelineLayoutCreateInfo              &getLayoutInfo()        const { return _layoutInfo; }
    const vk::PipelineMultisampleStateCreateInfo    &getMultisampleInfo()   const { return _multisampleState; }
    const vk::PipelineRasterizationStateCreateInfo  &getRasterizationInfo() const { return _rasterization; }
    const vk::PipelineVertexInputStateCreateInfo    &getVertexInputInfo()   const { return _vertexInput; }
    const vk::PipelineViewportStateCreateInfo       &getViewportInfo()      const { return _viewportState; }
    const vk::Extent2D                              &getResolution()        const { return _resolution; }
    const vk::Rect2D                                &getScissor()           const { return _scissor; }
    const vk::Viewport                              &getViewport()          const { return _viewport; }

    void setVertexInputInfo(const std::vector<vk::VertexInputBindingDescription> &bindingInfo,
            const std::vector<vk::VertexInputAttributeDescription> &attributeInfo);
    void setLayout(const PipelineLayoutInfo &pipelineLayoutInfo);
    void setDepthCompareOp(vk::CompareOp op);
    void setDepthBias(vk::Bool32 depthBiasEnable, float depthBiasConstantFactor);
    void setColorBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState> &attachments);

private:
    std::vector<vk::PipelineColorBlendAttachmentState>  _colorBlendAttachments;
    vk::PipelineColorBlendStateCreateInfo               _colorBlend;
    vk::PipelineDepthStencilStateCreateInfo             _depthStencil;
    std::vector<vk::DynamicState>                       _dynamicStates;
    vk::PipelineDynamicStateCreateInfo                  _dynamicState;
    vk::PipelineInputAssemblyStateCreateInfo            _inputAssembly;
    vk::PipelineLayoutCreateInfo                        _layoutInfo;
    vk::PipelineMultisampleStateCreateInfo              _multisampleState;
    vk::PipelineRasterizationStateCreateInfo            _rasterization;
    vk::PipelineVertexInputStateCreateInfo              _vertexInput;
    vk::PipelineViewportStateCreateInfo                 _viewportState;
    vk::Extent2D                                        _resolution;
    vk::Rect2D                                          _scissor;
    vk::Viewport                                        _viewport;
};