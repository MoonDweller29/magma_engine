#pragma once
#include <vulkan/vulkan.h>
#include "shaderModule.h"
#include <vector>

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
    VkPipelineColorBlendAttachmentState colorBlendAttachment;
    VkPipelineColorBlendStateCreateInfo colorBlending;
    std::vector<VkDynamicState> dynamicStates;
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkPipelineLayoutCreateInfo pipelineLayoutInfo;

public:
    PipelineInfo(VkExtent2D extent);
    ~PipelineInfo() = default;

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
    const VkPipelineColorBlendAttachmentState &getColorBlendAttachment() const { return colorBlendAttachment; }
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
    GraphicsPipeline(VkDevice device, const PipelineInfo &pipelineSettings, VkRenderPass renderPass);
    ~GraphicsPipeline();

    VkPipeline getHandler() const { return graphicsPipeline; }
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
};