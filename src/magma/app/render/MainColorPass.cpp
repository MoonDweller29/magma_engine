#include "magma/app/render/MainColorPass.h"

#include <array>

#include "magma/app/render/GBuffer.h"
#include "magma/app/scene/mesh.h"

MainColorPass::MainColorPass(vk::Device device, const GBuffer &gBuffer, Queue queue) :
    _device(device), _gBuffer(gBuffer), _queue(queue),
    _cmdBuf(device, queue.cmdPool),
    _renderFinished(device),
    _renderPass(std::move(createRenderPass())),
    _frameBuffer(_device,
                 {_gBuffer.getAlbedo().getView(), _gBuffer.getNormals().getView(),
                  _gBuffer.getGlobalPos().getView(), _gBuffer.getDepth().getView()},
                 _renderPass.get(), _gBuffer.getExtent()),
    _descriptorSetLayout(_device, DescriptorSetLayoutInfo()
        .addUniformBuffer(1, vk::ShaderStageFlagBits::eVertex)
        .addCombinedImageSampler(vk::ShaderStageFlagBits::eFragment)
    )
{
    PipelineLayoutInfo pipelineLayoutInfo(_descriptorSetLayout.getLayout());
    PipelineVertexInputInfo pipelineVertexInputInfo(Vertex::getBindingDescription(), Vertex::getAttributeDescriptions());

    PipelineInfo pipelineInfo(_gBuffer.getExtent());
    pipelineInfo.setVertexInputInfo(pipelineVertexInputInfo);
    pipelineInfo.setLayout(pipelineLayoutInfo);
    pipelineInfo.setDepthCompareOp(vk::CompareOp::eLessOrEqual);
    pipelineInfo.setColorBlendAttachments({
        PipelineInfo::createColorBlendAttachmentState(),
        PipelineInfo::createColorBlendAttachmentState(),
        PipelineInfo::createColorBlendAttachmentState(),
    });

    Shader vertShader("mainColorPassVert", _device, "shaders/mainColorPass.vert.spv", Shader::Stage::VERT_SH);
    Shader fragShader("mainColorPassFrag", _device, "shaders/mainColorPass.frag.spv", Shader::Stage::FRAG_SH);
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
            vertShader.getStageInfo(),
            fragShader.getStageInfo()
    };
    _graphicsPipeline = std::make_unique<GraphicsPipeline>(_device, shaderStages, pipelineInfo, _renderPass.get());
}

vk::UniqueRenderPass MainColorPass::createRenderPass() {
    vk::AttachmentDescription albedoAttachment;
    albedoAttachment.format         = _gBuffer.getAlbedo().getInfo()->imageInfo.format;
    albedoAttachment.samples        = vk::SampleCountFlagBits::e1; //for multisampling
    albedoAttachment.loadOp         = vk::AttachmentLoadOp::eClear;
    albedoAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
    albedoAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    albedoAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    albedoAttachment.initialLayout  = vk::ImageLayout::eUndefined;
    albedoAttachment.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;

    vk::AttachmentDescription normalsAttachment = albedoAttachment;
    normalsAttachment.format = _gBuffer.getNormals().getInfo()->imageInfo.format;
    vk::AttachmentDescription globalPosAttachment = albedoAttachment;
    globalPosAttachment.format = _gBuffer.getGlobalPos().getInfo()->imageInfo.format;

    std::array<vk::AttachmentReference, 3> colorAttachmentRef {
            vk::AttachmentReference(0, vk::ImageLayout::eColorAttachmentOptimal),
            vk::AttachmentReference(1, vk::ImageLayout::eColorAttachmentOptimal),
            vk::AttachmentReference(2, vk::ImageLayout::eColorAttachmentOptimal)
    };


    vk::AttachmentDescription depthAttachment;
    depthAttachment.format          = _gBuffer.getDepth().getInfo()->imageInfo.format;
    depthAttachment.samples         = vk::SampleCountFlagBits::e1;
    depthAttachment.loadOp          = vk::AttachmentLoadOp::eLoad;
    depthAttachment.storeOp         = vk::AttachmentStoreOp::eStore; //@TODO: try with store
    depthAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    depthAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    depthAttachment.initialLayout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depthAttachment.finalLayout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;

    vk::AttachmentReference depthAttachmentRef(3, vk::ImageLayout::eDepthStencilAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount    = colorAttachmentRef.size();
    subpass.pColorAttachments       = colorAttachmentRef.data();
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    std::array<vk::AttachmentDescription, 4> attachments = {
            albedoAttachment, normalsAttachment, globalPosAttachment, depthAttachment
    };

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

void MainColorPass::writeDescriptorSets(
        const Buffer &uniformBuffer, uint32_t uboSize,
        vk::ImageView albedoTexView, vk::Sampler sampler
) {
    _descriptorSetLayout.allocateSets(1);
    _descriptorSetLayout.beginSet(0);
    {
        _descriptorSetLayout.bindUniformBuffer(0, uniformBuffer.getBuf(), 0, uboSize);
        _descriptorSetLayout.bindCombinedImageSampler(1, albedoTexView, sampler);
    }
    _descriptorSet = _descriptorSetLayout.recordAndReturnSets()[0];
}

void MainColorPass::recordCmdBuffers(vk::Buffer indexBuffer, vk::Buffer vertexBuffer, uint32_t vertexCount) {
    vk::CommandBuffer cmdBuf = _cmdBuf.begin();
    {
        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.renderPass = _renderPass.get();
        renderPassInfo.framebuffer = _frameBuffer.getFrameBuf();

        renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
        renderPassInfo.renderArea.extent = _gBuffer.getExtent();

        std::array<vk::ClearValue, 4> clearValues;
        clearValues[0].setColor(std::array<float, 4>{0.0f,0.0f,0.0f,1.0f});
        clearValues[1].setColor(std::array<float, 4>{0.0f,0.0f,0.0f,1.0f});
        clearValues[2].setColor(std::array<float, 4>{0.0f,0.0f,0.0f,1.0f});
        clearValues[3].setDepthStencil(vk::ClearDepthStencilValue(1.0f, 0));

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

const CmdSync &MainColorPass::draw(
        const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences
) {
    vk::Result result;
    if (!waitFences.empty()) {
        result = _device.waitForFences(waitFences, VK_TRUE, UINT64_MAX);
    }

    vk::SubmitInfo submitInfo;

    std::vector<vk::PipelineStageFlags> waitStages(waitSemaphores.size(), vk::PipelineStageFlagBits::eEarlyFragmentTests);
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
