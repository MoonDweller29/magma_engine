#include "magma/app/render/GBufferResolve.h"

#include "magma/app/render/GBuffer.h"
#include "magma/vk/vulkan_common.h"

GBufferResolve::GBufferResolve(vk::Device device, Texture renderTarget, Queue queue) :
    _device(device), _renderTarget(renderTarget), _queue(queue),
    _extent(toExtent2D(_renderTarget.getInfo()->imageInfo.extent)),
    _cmdBuf(device, queue.cmdPool),
    _renderFinished(device),
    _imgSampler(createImageSampler()),
    _renderPass(std::move(createRenderPass())),
    _frameBuffer(_device, {_renderTarget.getView()},
                 _renderPass.get(), _extent)
{
    initDescriptorSetLayout();

    PipelineLayoutInfo pipelineLayoutInfo(_descriptorSetLayout.getLayout());
    PipelineInfo pipelineInfo(_extent);
    pipelineInfo.setLayout(pipelineLayoutInfo);

    Shader vertShader("imageProcessVert", _device, "shaders/imageProcess.vert.spv", Shader::Stage::VERT_SH);
    Shader fragShader("gBufferResolveFrag", _device, "shaders/gBufferResolve.frag.spv", Shader::Stage::FRAG_SH);
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = {
            vertShader.getStageInfo(),
            fragShader.getStageInfo()
    };
    _graphicsPipeline = std::make_unique<GraphicsPipeline>(_device, shaderStages, pipelineInfo, _renderPass.get());
}

void GBufferResolve::initDescriptorSetLayout() {
    _descriptorSetLayout.addCombinedImageSampler(vk::ShaderStageFlagBits::eFragment);
    _descriptorSetLayout.addCombinedImageSampler(vk::ShaderStageFlagBits::eFragment);
    _descriptorSetLayout.addCombinedImageSampler(vk::ShaderStageFlagBits::eFragment);
    _descriptorSetLayout.addCombinedImageSampler(vk::ShaderStageFlagBits::eFragment);
    _descriptorSetLayout.addUniformBuffer(1, vk::ShaderStageFlagBits::eFragment);
    _descriptorSetLayout.addUniformBuffer(1, vk::ShaderStageFlagBits::eFragment);
    _descriptorSetLayout.createLayout(_device);
}

vk::UniqueRenderPass GBufferResolve::createRenderPass() {
    vk::AttachmentDescription colorAttachment;
    colorAttachment.format         = _renderTarget.getInfo()->imageInfo.format;
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

vk::UniqueSampler GBufferResolve::createImageSampler() {
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.magFilter = vk::Filter::eNearest;
    samplerInfo.minFilter = vk::Filter::eNearest;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eRepeat;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eRepeat;
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


void GBufferResolve::writeDescriptorSets(
        const GBuffer &gBuffer,
        vk::ImageView shadowMapView, vk::Sampler shadowMapSampler,
        const Buffer &fragmentUniform, uint32_t fuboSize,
        const Buffer &lightSpaceUniform, uint32_t luboSize
) {
    _descriptorSetLayout.allocateSets(1);
    _descriptorSetLayout.beginSet(0);
    {
        _descriptorSetLayout.bindCombinedImageSampler(0, gBuffer.getAlbedo().getView(), _imgSampler.get());
        _descriptorSetLayout.bindCombinedImageSampler(1, gBuffer.getNormals().getView(), _imgSampler.get());
        _descriptorSetLayout.bindCombinedImageSampler(2, gBuffer.getGlobalPos().getView(), _imgSampler.get());
        _descriptorSetLayout.bindCombinedImageSampler(3, shadowMapView, shadowMapSampler);
        _descriptorSetLayout.bindUniformBuffer(4, fragmentUniform.getBuf(), 0, fuboSize);
        _descriptorSetLayout.bindUniformBuffer(5, lightSpaceUniform.getBuf(), 0, luboSize);
    }
    _descriptorSet = _descriptorSetLayout.recordAndReturnSets()[0];
}

void GBufferResolve::recordCmdBuffers() {
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

const CmdSync &GBufferResolve::draw(
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