#include "magma/app/render/DepthPass.h"

#include <array>

#include "magma/vk/ShaderModule.h"
#include "magma/app/scene/mesh.h"


DepthPass::DepthPass(vk::Device device, const Texture &depthTex, vk::ImageLayout depthFinalLayout, Queue queue) :
    _device(device),
    _queue(queue),
    _depthTex(depthTex),
    _depthFinalLayout(depthFinalLayout),
    _extent(toExtent2D(_depthTex.getInfo()->imageInfo.extent)),
    _cmdBuf(_device, _queue.cmdPool),
    _renderFinished(_device),
    _renderPass(std::move(createRenderPass())),
    _descriptorSetLayout(_device, DescriptorSetLayoutInfo()
        .addUniformBuffer(1, vk::ShaderStageFlagBits::eVertex)
    )
{
    PipelineInfo pipelineInfo(_extent);
    PipelineVertexInputInfo pipelineVertexInputInfo(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions());
    pipelineInfo.setVertexInputInfo(pipelineVertexInputInfo);
    PipelineLayoutInfo pipelineLayoutInfo(_descriptorSetLayout.getLayout());
    pipelineInfo.setLayout(pipelineLayoutInfo);

    Shader vertShader("depthVertShader", _device, "shaders/depth.vert.spv", Shader::Stage::VERT_SH);
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = { vertShader.getStageInfo() };
    _graphicsPipeline = std::make_unique<GraphicsPipeline>(_device, shaderStages, pipelineInfo, _renderPass.get());

    std::vector<vk::ImageView> attachments = { depthTex.getView() };
    _frameBuffer = std::make_unique<FrameBuffer>(_device, attachments, _renderPass.get(), _extent);
}

vk::UniqueRenderPass DepthPass::createRenderPass() {
    vk::AttachmentDescription depthAttachment;
    depthAttachment.format = _depthTex.getInfo()->imageInfo.format;
    depthAttachment.samples = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp = vk::AttachmentLoadOp::eClear;
    depthAttachment.storeOp = vk::AttachmentStoreOp::eStore;
    depthAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout = vk::ImageLayout::eUndefined;
    depthAttachment.finalLayout = _depthFinalLayout;

    vk::AttachmentReference depthAttachmentRef(0, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount = 0;
    subpass.pColorAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<vk::AttachmentDescription, 1> attachments = { depthAttachment };
    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    //external dependency subpass
    vk::SubpassDependency dependency;
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0; //this subpass
    dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlags();
    dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    auto [result, renderPass] = _device.createRenderPassUnique(renderPassInfo);
    VK_CHECK_ERR(result, "failed to create render pass!");

    return std::move(renderPass);
}

void DepthPass::writeDescriptorSets(const Buffer &uniformBuffer, uint32_t uboSize) {
    _descriptorSetLayout.allocateSets(1);
    _descriptorSetLayout.beginSet(0);
    _descriptorSetLayout.bindUniformBuffer(0, uniformBuffer.getBuf(), 0, uboSize);
    _descriptorSet = _descriptorSetLayout.recordAndReturnSets()[0];
}

void DepthPass::recordCmdBuffers(vk::Buffer indexBuffer, vk::Buffer vertexBuffer, uint32_t vertexCount) {
    vk::CommandBuffer cmdBuf = _cmdBuf.begin();
    {
        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.renderPass = _renderPass.get();
        renderPassInfo.framebuffer = _frameBuffer->getFrameBuf();

        renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
        renderPassInfo.renderArea.extent = _extent;

        std::array<vk::ClearValue, 1> clearValues{};
        clearValues[0].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0));

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        cmdBuf.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        {
            cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, _graphicsPipeline->getPipeline());

            vk::Buffer vertexBuffers[] = { vertexBuffer };
            vk::DeviceSize offsets[] = { 0 };
            cmdBuf.bindVertexBuffers(0, 1, vertexBuffers, offsets);
            cmdBuf.bindIndexBuffer(indexBuffer, 0, vk::IndexType::eUint32);
            cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                      _graphicsPipeline->getPipelineLayout(), 0, 1, &_descriptorSet, 0, nullptr);

            cmdBuf.drawIndexed(vertexCount, 1, 0, 0, 0);
        }
        cmdBuf.endRenderPass();
    }
    _cmdBuf.end();
}

CmdSync DepthPass::draw(
        const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences
) {
    vk::Result result;
    if (!waitFences.empty()) {
        result = _device.waitForFences(waitFences, VK_TRUE, UINT64_MAX);
    }

    vk::SubmitInfo submitInfo;

    std::vector<vk::PipelineStageFlags> waitStages(waitSemaphores.size(), vk::PipelineStageFlagBits::eColorAttachmentOutput);
    if (waitSemaphores.empty()) {
        submitInfo.waitSemaphoreCount = 0;
        submitInfo.pWaitSemaphores = nullptr;
    } else if (
            std::find(waitSemaphores.begin(), waitSemaphores.end(), _renderFinished.getSemaphore()) == waitSemaphores.end()
            ) {
        submitInfo.waitSemaphoreCount = waitSemaphores.size();
        submitInfo.pWaitSemaphores = waitSemaphores.data();

        submitInfo.pWaitDstStageMask = waitStages.data();
    } else {
        LOG_AND_THROW std::logic_error("cyclic dependency in render graph");
    }

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &_cmdBuf.getCmdBuf();

    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &_renderFinished.getSemaphore();

    _renderFinished.resetFence();

    result = _queue.queue.submit(1, &submitInfo, _renderFinished.getFence());
    VK_CHECK_ERR(result, "failed to submit draw command buffer!");

    return _renderFinished;
}
