#include "magma/app/render/colorPass.h"

#include "magma/vk/shaderModule.h"
#include "magma/vk/vulkan_common.h"

ColorPass::ColorPass(LogicalDevice &device, SwapChain &swapChain):
    device(device), swapChain(swapChain),
    extent(swapChain.getExtent()),
    _commandBuffers(device.getDevice(), device.getGraphicsCmdPool(), swapChain.imgCount())
{
    initDescriptorSetLayout();
    createRenderPass();

    PipelineInfo pipelineInfo(extent);
    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();
    pipelineInfo.setVertexInputInfo(bindingDescription, attributeDescriptions);
    pipelineInfo.setLayout(descriptorSetLayout.getLayout());
    pipelineInfo.setDepthCompareOp(VK_COMPARE_OP_LESS_OR_EQUAL);

    Shader vertShader(device.c_getDevice(), "shaders/shader.vert.spv", Shader::VERT_SH);
    Shader fragShader(device.c_getDevice(), "shaders/shader.frag.spv", Shader::FRAG_SH);
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages = {
            vertShader.getStageInfo(),
            fragShader.getStageInfo()
    };
    graphicsPipeline = std::make_unique<GraphicsPipeline>(device.c_getDevice(), shaderStages, pipelineInfo, renderPass);

    renderFinished.create(device.c_getDevice());
}

void ColorPass::initDescriptorSetLayout()
{
    descriptorSetLayout.addUniformBuffer(1, VK_SHADER_STAGE_VERTEX_BIT);
    descriptorSetLayout.addCombinedImageSampler(VK_SHADER_STAGE_FRAGMENT_BIT);
    descriptorSetLayout.addUniformBuffer(1, VK_SHADER_STAGE_FRAGMENT_BIT);
    descriptorSetLayout.addUniformBuffer(1, VK_SHADER_STAGE_VERTEX_BIT);
    descriptorSetLayout.addCombinedImageSampler(VK_SHADER_STAGE_FRAGMENT_BIT);
    descriptorSetLayout.createLayout(device.c_getDevice());
}

void ColorPass::writeDescriptorSets(const Buffer &uniformBuffer, uint32_t ubo_size,
                                    const Buffer &fragmentUniform, uint32_t fu_size,
                                    VkImageView tex_view, VkSampler sampler,
                                    const Buffer &lightSpaceUniform, uint32_t lu_size,
                                    VkImageView shadow_map_view, VkSampler shadow_sampler)
{
    uint32_t descriptorSetCount = swapChain.imgCount();
    descriptorSetLayout.allocateSets(descriptorSetCount);
    for (uint32_t i = 0; i < descriptorSetCount; ++i)
    {
        descriptorSetLayout.beginSet(i);
        descriptorSetLayout.bindUniformBuffer(0, uniformBuffer.getBuf(), 0, ubo_size);
        descriptorSetLayout.bindCombinedImageSampler(1, tex_view, sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        descriptorSetLayout.bindUniformBuffer(2, fragmentUniform.getBuf(), 0, fu_size);
        descriptorSetLayout.bindUniformBuffer(3, lightSpaceUniform.getBuf(), 0, lu_size);
        descriptorSetLayout.bindCombinedImageSampler(4, shadow_map_view, shadow_sampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    }
    descriptorSets = descriptorSetLayout.recordAndReturnSets();
}

void ColorPass::recordCmdBuffers(
        VkBuffer indexBuffer,
        VkBuffer vertexBuffer,
        uint32_t vertexCount,
        const std::vector<VkFramebuffer> &frameBuffers
)
{
    for (size_t i = 0; i < swapChain.imgCount(); ++i)
    {
        VkCommandBuffer cmdBuf = _commandBuffers.begin(i);
        {
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChain.getVkFrameBuffers()[i];

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChain.getExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(cmdBuf, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            {
                vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline->getHandler());

                VkBuffer vertexBuffers[] = {vertexBuffer};
                VkDeviceSize offsets[] = {0};
                vkCmdBindVertexBuffers(cmdBuf, 0, 1, vertexBuffers, offsets);
                vkCmdBindIndexBuffer(cmdBuf, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdBindDescriptorSets(cmdBuf,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        graphicsPipeline->getPipelineLayout(), 0, 1, &descriptorSets[i], 0, nullptr);

                vkCmdDrawIndexed(cmdBuf, vertexCount, 1, 0, 0, 0);
            }
            vkCmdEndRenderPass(cmdBuf);
        }
        _commandBuffers.end(i);
    }
}

void ColorPass::createRenderPass()
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = swapChain.getImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //for multisampling
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat(device.getVkPhysDevice());
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
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

CmdSync ColorPass::draw(
        uint32_t i,
        const std::vector<VkSemaphore> &waitSemaphores,
        const std::vector<VkFence> &waitFences
)
{
    if (waitFences.size() > 0)
    {
        vkWaitForFences(device.c_getDevice(), waitFences.size(), waitFences.data(), VK_TRUE, UINT64_MAX);
    }

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    std::vector<VkPipelineStageFlags> waitStages(waitSemaphores.size(), VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT);
    if (waitSemaphores.size() == 0)
    {
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
    }
    else if (waitSemaphores[0] != renderFinished.semaphore)
    {
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.data();
    }
    submitInfo.pWaitDstStageMask = waitStages.data();

    submitInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer = (VkCommandBuffer)_commandBuffers[i];
    submitInfo.pCommandBuffers = &commandBuffer;

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderFinished.semaphore;

    vkResetFences(device.c_getDevice(), 1, &renderFinished.fence);

    VkResult result = vkQueueSubmit(device.getGraphicsQueue(), 1, &submitInfo, renderFinished.fence);
    VK_CHECK_ERR(result, "failed to submit draw command buffer!");

    return renderFinished;
}

ColorPass::~ColorPass()
{
    vkDestroyRenderPass(device.c_getDevice(), renderPass, nullptr);
    graphicsPipeline.reset();
    descriptorSetLayout.clear();
}
