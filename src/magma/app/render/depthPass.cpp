#include "magma/app/render/depthPass.h"

#include <array>

#include "magma/vk/ShaderModule.h"
#include "magma/vk/vulkan_common.h"

DepthPass::DepthPass(LogicalDevice &device, const Texture &depthTex, VkExtent2D extent, VkImageLayout depthFinalLayout)
        : device(device),
        depthTex(depthTex),
        extent(extent),
        depthFinalLayout(depthFinalLayout), 
        _commandBuffer(device.c_getDevice(), device.getGraphicsQueue().cmdPool),
        renderFinished(device.getDevice())
{
    initDescriptorSetLayout();
    createRenderPass();
    vk::DescriptorSetLayout vk_descriptorSetLayout = vk::DescriptorSetLayout(descriptorSetLayout.getLayout());
    PipelineLayoutInfo pipelineLayoutInfo(vk_descriptorSetLayout);
    PipelineInfo pipelineInfo(extent);
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
    pipelineInfo.setLayout(pipelineLayoutInfo);

    Shader vertShader("depthVertShader", device.getDevice(), "shaders/depth.vert.spv", Shader::Stage::VERT_SH);
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = { vertShader.getStageInfo() };
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device.getDevice(), shaderStages, pipelineInfo, vk::RenderPass(renderPass));

    std::vector<vk::ImageView> attachments = { depthTex.getView() };
    frameBuffer = std::make_unique<FrameBuffer>(device.c_getDevice(), attachments, renderPass, extent);
}

void DepthPass::initDescriptorSetLayout()
{
    descriptorSetLayout.addUniformBuffer(1, VK_SHADER_STAGE_VERTEX_BIT);
    descriptorSetLayout.createLayout(device.c_getDevice());
}

void DepthPass::writeDescriptorSets(const Buffer &uniformBuffer, uint32_t ubo_size)
{
    descriptorSetLayout.allocateSets(1);
    descriptorSetLayout.beginSet(0);
    descriptorSetLayout.bindUniformBuffer(0, uniformBuffer.getBuf(), 0, ubo_size);
    descriptorSet = descriptorSetLayout.recordAndReturnSets()[0];
}

void DepthPass::createRenderPass()
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = (VkFormat)depthTex.getInfo()->imageInfo.format;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = depthFinalLayout;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 0;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 0;
    subpass.pColorAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 1> attachments = { depthAttachment };
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    //external dependency subpass
    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0; //this subpass
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    VkResult result = vkCreateRenderPass(device.c_getDevice(), &renderPassInfo, nullptr, &renderPass);
    VK_CHECK_ERR(result, "failed to create render pass!");
}

void DepthPass::recordCmdBuffers(
        VkBuffer indexBuffer,
        VkBuffer vertexBuffer,
        uint32_t vertexCount)
{
    VkCommandBuffer cmdBuf = _commandBuffer.begin();
    {
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = frameBuffer->getFrameBuf();

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;

        std::array<VkClearValue, 1> clearValues{};
        clearValues[0].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(cmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        {
            vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)graphicsPipeline->getPipeline());

            VkBuffer vertexBuffers[] = {vertexBuffer};
            VkDeviceSize offsets[] = {0};
            vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
            vkCmdBindIndexBuffer(cmdBuf, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
            vkCmdBindDescriptorSets(cmdBuf,
                                    VK_PIPELINE_BIND_POINT_GRAPHICS,
                                    graphicsPipeline->getPipelineLayout(), 0, 1, &descriptorSet, 0, nullptr);

            vkCmdDrawIndexed(cmdBuf, vertexCount, 1, 0, 0, 0);
        }
        vkCmdEndRenderPass(cmdBuf);
    }
    _commandBuffer.end();
}

CmdSync DepthPass::draw(
        const std::vector<VkSemaphore> &waitSemaphores,
        const std::vector<VkFence> &waitFences)
{
    if (waitFences.size() > 0)
    {
        vkWaitForFences(device.c_getDevice(), waitFences.size(), waitFences.data(), VK_TRUE, UINT64_MAX);
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::vector<VkPipelineStageFlags> waitStages(waitSemaphores.size(), VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT);
    if (waitSemaphores.size() == 0)
    {
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
    }
    else if (waitSemaphores[0] != renderFinished.getSemaphore())
    {
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.data();
    }
    submitInfo.pWaitDstStageMask = waitStages.data();

    submitInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer = _commandBuffer.c_getCmdBuf();
    submitInfo.pCommandBuffers = &commandBuffer;

    submitInfo.signalSemaphoreCount = 1;
    VkSemaphore renderFinishedSemaphore = renderFinished.getSemaphore();
    submitInfo.pSignalSemaphores = &renderFinishedSemaphore;

    renderFinished.resetFence();

    VkResult result = vkQueueSubmit(device.getGraphicsQueue().queue, 1, &submitInfo, renderFinished.getFence());
    VK_CHECK_ERR(result, "failed to submit draw command buffer!");

    return renderFinished;
}

DepthPass::~DepthPass()
{
    vkDestroyRenderPass(device.c_getDevice(), renderPass, nullptr);
    graphicsPipeline.reset();
    descriptorSetLayout.clear();
}
