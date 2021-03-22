#pragma once
#include <vulkan/vulkan.hpp>
#include <vector>

class PipelineLayoutInfo {
public:
    PipelineLayoutInfo();
    PipelineLayoutInfo(const std::vector<vk::DescriptorSetLayout> &descriptorSetLayouts);
    PipelineLayoutInfo(const vk::DescriptorSetLayout &descriptorSetLayout);

    ~PipelineLayoutInfo() = default;

    const vk::PipelineLayoutCreateInfo &getInfo() const { return _pipelineLayoutInfo; }

private:
    std::vector<vk::DescriptorSetLayout> _descriptorSetLayouts;
    vk::PipelineLayoutCreateInfo _pipelineLayoutInfo;
};

class PipelineVertexInputInfo {
public:
    PipelineVertexInputInfo() = default;
    PipelineVertexInputInfo(const std::vector<vk::VertexInputBindingDescription> &bindingInfo,
        const std::vector<vk::VertexInputAttributeDescription> &attributeInfo);

    ~PipelineVertexInputInfo() = default;

    const vk::PipelineVertexInputStateCreateInfo &getInfo() const { return _pipelineVertexInputInfo; }
private:
    std::vector<vk::VertexInputBindingDescription> _bindingInfo;
    std::vector<vk::VertexInputAttributeDescription> _attributeInfo;
    vk::PipelineVertexInputStateCreateInfo _pipelineVertexInputInfo;
};


class PipelineInfo {
public:
    PipelineInfo(vk::Extent2D extent);
    ~PipelineInfo() = default;

    static vk::PipelineColorBlendAttachmentState createColorBlendAttachmentState();

    const vk::Extent2D                              &getResolution()        const { return _resolution;             }
    const vk::PipelineViewportStateCreateInfo       &getViewportInfo()      const { return _viewportState;          }
    const vk::Viewport                              &getViewport()          const { return _viewport;               }
    const vk::Rect2D                                &getScissor()           const { return _scissor;                }

    const vk::PipelineLayoutCreateInfo              &getLayoutInfo()        const { return _layoutInfo.getInfo();   }

    const vk::PipelineVertexInputStateCreateInfo    &getVertexInputInfo()   const { return _vertexInfo.getInfo();   }
    const vk::PipelineInputAssemblyStateCreateInfo  &getAssemblyInfo()      const { return _inputAssembly;          }

    const vk::PipelineDepthStencilStateCreateInfo   &getDepthStencilInfo()  const { return _depthStencil;           }

    const vk::PipelineMultisampleStateCreateInfo    &getMultisampleInfo()   const { return _multisampleState;       }
    const vk::PipelineRasterizationStateCreateInfo  &getRasterizationInfo() const { return _rasterizer;             }

    const vk::PipelineColorBlendStateCreateInfo     &getColorBlendInfo()    const { return _colorBlend;             }

    const vk::PipelineDynamicStateCreateInfo        &getDynamicStateInfo()  const { return _dynamicState;           }

    void setVertexInputInfo(const PipelineVertexInputInfo &pipelineVertexInputInfo);
    void setLayout(const PipelineLayoutInfo &pipelineLayoutInfo);
    void setDepthCompareOp(vk::CompareOp op);
    void setDepthBias(vk::Bool32 depthBiasEnable, float depthBiasConstantFactor);
    void setColorBlendAttachments(const std::vector<vk::PipelineColorBlendAttachmentState> &attachments);

private:
    vk::Extent2D                                        _resolution;
    vk::PipelineViewportStateCreateInfo                 _viewportState;
    vk::Viewport                                        _viewport;
    vk::Rect2D                                          _scissor;

    PipelineLayoutInfo                                  _layoutInfo;

    PipelineVertexInputInfo                             _vertexInfo;
    vk::PipelineInputAssemblyStateCreateInfo            _inputAssembly;

    vk::PipelineDepthStencilStateCreateInfo             _depthStencil;

    vk::PipelineMultisampleStateCreateInfo              _multisampleState;
    vk::PipelineRasterizationStateCreateInfo            _rasterizer;

    std::vector<vk::PipelineColorBlendAttachmentState>  _colorBlendAttachments;
    vk::PipelineColorBlendStateCreateInfo               _colorBlend;

    std::vector<vk::DynamicState>                       _dynamicStates;
    vk::PipelineDynamicStateCreateInfo                  _dynamicState;
};