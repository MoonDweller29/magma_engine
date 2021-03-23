#pragma once
#include <vulkan/vulkan.h>
#include <vector>

#include "magma/vk/ShaderModule.h"

class PipelineInfo
{
    VkExtent2D resolution;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo;
    VkPipelineInputAssemblyStateCreateInfo inputAssembly;
    VkViewport viewport;
    VkRect2D scissor;
    VkPipelineViewportStateCreateInfo viewportState;
    VkPipelineRasterizationStateCreateInfo rasterizer;
    VkPipelineMultisampleStateCreateInfo multisampling;
    VkPipelineDepthStencilStateCreateInfo depthStencil;
    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    VkPipelineColorBlendStateCreateInfo colorBlending;
    std::vector<VkDynamicState> dynamicStates;
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;

public:
    PipelineInfo(VkExtent2D extent);
    ~PipelineInfo() = default;

    static VkPipelineColorBlendAttachmentState createColorBlendAttachmentOFF();

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    // getters
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    const VkExtent2D &getRes()                                           const { return resolution; }
    const VkPipelineVertexInputStateCreateInfo &getVertexInputInfo()     const { return vertexInputInfo; }
    const VkPipelineInputAssemblyStateCreateInfo &getInputAssembly()     const { return inputAssembly; }
    const VkViewport &getViewport()                                      const { return viewport; }
    const VkRect2D &getScissor()                                         const { return scissor; }
    const VkPipelineViewportStateCreateInfo &getViewportState()          const { return viewportState; }
    const VkPipelineRasterizationStateCreateInfo &getRasterizer()        const { return rasterizer; }
    const VkPipelineMultisampleStateCreateInfo &getMultisampling()       const { return multisampling; }
    const VkPipelineDepthStencilStateCreateInfo &getDepthStencil()       const { return depthStencil; }
    const VkPipelineColorBlendStateCreateInfo &getColorBlending()        const { return colorBlending; }
    const VkPipelineDynamicStateCreateInfo &getDynamicState()            const { return dynamicState; }
    const VkPipelineLayoutCreateInfo &getPipelineLayoutInfo()            const { return pipelineLayoutInfo; }
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //set settings
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    void setVertexInputInfo(
            const std::vector<VkVertexInputBindingDescription> &bindingInfo,
            const std::vector<VkVertexInputAttributeDescription> &attributeInfo
    );
    void setLayouts(const std::vector<VkDescriptorSetLayout> &descriptorSetLayouts);
    void setLayout(const VkDescriptorSetLayout &descriptorSetLayout);
    void setDepthCompareOp(VkCompareOp op);
    void setDepthBias(VkBool32 depthBiasEnable, float depthBiasConstantFactor);
    void setColorBlendAttachments(const std::vector<VkPipelineColorBlendAttachmentState> &attachments);
    //////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //////////////////////////////////////////////////////////////////////////////////////////////////////
};

class GraphicsPipeline
{
    VkDevice device;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
public:
    GraphicsPipeline(
            VkDevice device,
            const std::vector<VkPipelineShaderStageCreateInfo> &shaderStages,
            const PipelineInfo &pipelineSettings, VkRenderPass renderPass);
    ~GraphicsPipeline();

    VkPipeline getHandler() const { return graphicsPipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
};