#include "magma/app/render/blur/Gauss1D.h"


Gauss1D::Gauss1D(vk::Device device, Texture inpTex, Texture outTex, int axis, Queue queue) :
        _device(device), _outTex(outTex), _axis(axis), _queue(queue),
        _extent(toExtent2D(_outTex.getInfo()->imageInfo.extent)),
        _cmdBuf(device, queue.cmdPool),
        _renderFinished(device),
        _imageSampler(createImageSampler()),
        _renderPass(std::move(createRenderPass())),
        _frameBuffer(_device, {_outTex.getView()},
                     _renderPass.get(), _extent),
        _descriptorSetLayout(_device, DescriptorSetLayoutInfo()
                .addCombinedImageSampler(vk::ShaderStageFlagBits::eFragment)
        )
{
    PipelineLayoutInfo pipelineLayoutInfo(_descriptorSetLayout.getLayout());
    PipelineInfo pipelineInfo(_extent);
    pipelineInfo.setLayout(pipelineLayoutInfo);

    Shader vertShader("imageProcessVert", _device, "shaders/imageProcess.vert.spv", Shader::Stage::VERT_SH);
    std::string fragName;
    std::string fragPath;
    if (_axis == 0) {
        fragName = "gauss_v";
        fragPath = "shaders/blur/gauss_v.frag.spv";
    } else if (_axis == 1) {
        fragName = "gauss_h";
        fragPath = "shaders/blur/gauss_h.frag.spv";
    }
    Shader fragShader(fragName, _device, fragPath, Shader::Stage::FRAG_SH);
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
            vertShader.getStageInfo(),
            fragShader.getStageInfo()
    };
    _graphicsPipeline = std::make_unique<GraphicsPipeline>(_device, shaderStages, pipelineInfo, _renderPass.get());
    writeDescriptorSets(inpTex.getView());
    recordCmdBuffers();
}

vk::UniqueRenderPass Gauss1D::createRenderPass() {
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format         = _outTex.getInfo()->imageInfo.format;
    colorAttachment.samples        = vk::SampleCountFlagBits::e1; //for multisampling
    colorAttachment.loadOp         = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.storeOp        = vk::AttachmentStoreOp::eStore;
    colorAttachment.stencilLoadOp  = vk::AttachmentLoadOp::eDontCare;
    colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
    colorAttachment.initialLayout  = vk::ImageLayout::eUndefined;
    colorAttachment.finalLayout    = vk::ImageLayout::eShaderReadOnlyOptimal;

    vk::AttachmentReference colorAttachmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);

    vk::SubpassDescription subpass;
    subpass.pipelineBindPoint       = vk::PipelineBindPoint::eGraphics;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = nullptr;

    vk::RenderPassCreateInfo renderPassInfo;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
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

vk::UniqueSampler Gauss1D::createImageSampler() {
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.magFilter = vk::Filter::eNearest;
    samplerInfo.minFilter = vk::Filter::eNearest;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;

    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = vk::CompareOp::eAlways;

    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    auto[result, sampler] = _device.createSamplerUnique(samplerInfo);
    VK_CHECK_ERR(result, "failed to create texture sampler!");

    return std::move(sampler);
}

void Gauss1D::writeDescriptorSets(vk::ImageView inpTexView) {
    _descriptorSetLayout.allocateSets(1);
    _descriptorSetLayout.beginSet(0);
    {
        _descriptorSetLayout.bindCombinedImageSampler(0, inpTexView, _imageSampler.get());
    }
    _descriptorSet = _descriptorSetLayout.recordAndReturnSets()[0];
}

void Gauss1D::recordCmdBuffers() {
    vk::CommandBuffer cmdBuf = _cmdBuf.begin();
    {
        vk::RenderPassBeginInfo renderPassInfo;
        renderPassInfo.renderPass = _renderPass.get();
        renderPassInfo.framebuffer = _frameBuffer.getFrameBuf();

        renderPassInfo.renderArea.offset = vk::Offset2D(0, 0);
        renderPassInfo.renderArea.extent = _extent;

        renderPassInfo.clearValueCount = 0;
        renderPassInfo.pClearValues = nullptr;

        cmdBuf.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
        {
            cmdBuf.bindPipeline(vk::PipelineBindPoint::eGraphics, _graphicsPipeline->getPipeline());
            cmdBuf.bindDescriptorSets(vk::PipelineBindPoint::eGraphics,
                                      _graphicsPipeline->getPipelineLayout(), 0, 1, &_descriptorSet, 0, nullptr);
            cmdBuf.draw(3, 1, 0, 0);
        }
        cmdBuf.endRenderPass();
    }
    _cmdBuf.end();
}

const CmdSync &Gauss1D::draw(
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