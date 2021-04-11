#include "magma/app/postProcess/TemporalReproject.h"
#include "magma/app/compute/ComputePass.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/buffers/BufferManager.h"
#include "magma/vk/descriptors/DescriptorSetLayoutInfo.h"
#include <iterator>
#include <memory>
#include <string>
#include <vulkan/vulkan.hpp>

TemporalReproject::TemporalReproject(LogicalDevice &device, const Texture &colorTexture, const Texture &depthTexture, glm::mat4 proj) 
        : _device(device),
        _targetIndex(0),
        _sourceColorTexture(colorTexture),
        _sourceDepthTexture(depthTexture),
        _sourceExtent(toExtent2D(colorTexture.getInfo()->imageInfo.extent)),
        _targetExtent(_sourceExtent.width * 2, _sourceExtent.height * 2),
        _proj(proj),
        _inverseProj(computeInverseProj(proj)),
        _jitterShift( {
            glm::ivec2(0, 1),
            glm::ivec2(0, 0),
            glm::ivec2(1, 0),
            glm::ivec2(1, 1)
        } ) {
    createUniformBuffers();
    createSourceSampler();
    createTargetTextures();

    DescriptorSetLayoutInfo computeLayoutInfoReproject;
    // computeLayoutInfoReproject.addCombinedImageSampler(vk::ShaderStageFlagBits::eCompute);
    // computeLayoutInfoReproject.addCombinedImageSampler(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoReproject.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoReproject.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoReproject.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoReproject.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoReproject.addUniformBuffer(1, vk::ShaderStageFlagBits::eCompute);

    Shader computeShaderReproject("temporalReproject.comp", _device.getDevice(), "shaders/temporalReproject.comp.spv", Shader::Stage::COMP_SH);

    DescriptorSetLayoutInfo computeLayoutInfoAddData;
    computeLayoutInfoAddData.addCombinedImageSampler(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoAddData.addCombinedImageSampler(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoAddData.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoAddData.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoAddData.addUniformBuffer(1, vk::ShaderStageFlagBits::eCompute);

    Shader computeShaderAddData("temporalAddData.comp", _device.getDevice(), "shaders/temporalAddData.comp.spv", Shader::Stage::COMP_SH);

    DescriptorSetLayoutInfo computeLayoutInfoResolve;
    computeLayoutInfoResolve.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfoResolve.addStorageImage(vk::ShaderStageFlagBits::eCompute);

    Shader computeShaderResolve("temporalResolve.comp", _device.getDevice(), "shaders/temporalResolve.comp.spv", Shader::Stage::COMP_SH);

    for (int i = 0; i < 2; i++) {
        _computeReproject.push_back(std::make_unique<ComputePass>(device, computeLayoutInfoReproject, computeShaderReproject));
        _computeAddData.push_back(std::make_unique<ComputePass>(device, computeLayoutInfoAddData, computeShaderAddData));
        _computeResolve.push_back(std::make_unique<ComputePass>(device, computeLayoutInfoResolve, computeShaderResolve));
    }
}

TemporalReproject::~TemporalReproject()
{
    _device.getDevice().destroySampler(_sourceSampler);
    for (int i = 0; i < 2; i++) {
        _device.getTextureManager().deleteTexture(_targetColorTextures[i]);
        _device.getTextureManager().deleteTexture(_targetDepthTextures[i]);
    }
}

void TemporalReproject::createSourceSampler() {
    vk::SamplerCreateInfo samplerInfo;
    samplerInfo.magFilter = vk::Filter::eNearest;
    samplerInfo.minFilter = vk::Filter::eNearest;
    samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToEdge;
    samplerInfo.anisotropyEnable = false;
    samplerInfo.borderColor = vk::BorderColor::eFloatOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = true;
    samplerInfo.compareEnable = false;
    samplerInfo.compareOp = vk::CompareOp::eAlways;
    samplerInfo.mipmapMode = vk::SamplerMipmapMode::eNearest;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    vk::Result result;
    std::tie(result, _sourceSampler) = _device.getDevice().createSampler(samplerInfo);
}

void TemporalReproject::createTargetTextures() {
    TextureManager & textureMenager = _device.getTextureManager();
    std::string nameColor = "temporalReproject.texColor";
    std::string nameDepth = "temporalReproject.texDepth";
    for (int i = 0; i < 2; i++) {
        _targetColorTextures.push_back(textureMenager.createTexture2D(nameColor + "_" + std::to_string(i), _targetColorTextureFormat, _targetExtent,
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage, vk::ImageAspectFlagBits::eColor));
        textureMenager.setLayout(_targetColorTextures[i], vk::ImageLayout::eGeneral);
        _targetDepthTextures.push_back(textureMenager.createTexture2D(nameDepth + "_" + std::to_string(i), _targetDepthTextureFormat, _targetExtent,
            vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage, vk::ImageAspectFlagBits::eColor));
        textureMenager.setLayout(_targetDepthTextures[i], vk::ImageLayout::eGeneral);
    }
    _targetResolveTexture = textureMenager.createTexture2D("temporalResolve.texColor", _targetResolveTextureFormat, _sourceExtent,
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage, vk::ImageAspectFlagBits::eColor);
}

std::vector<vk::DescriptorSet> TemporalReproject::writeDescriptorSetReproject(int i) {
    DescriptorSetLayout& descriptorSetLayout = _computeReproject[i]->getDecriptorSetLayaut();
    descriptorSetLayout.allocateSets(1);
    descriptorSetLayout.beginSet(0);
    descriptorSetLayout.bindStorageImage(0, _targetColorTextures[(i + 1) % 2].getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindStorageImage(1, _targetDepthTextures[(i + 1) % 2].getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindStorageImage(2, _targetColorTextures[i].getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindStorageImage(3, _targetDepthTextures[i].getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindUniformBuffer(4, _transformBuffer.getBuf(), 0, sizeof(UBOTransformMatrix));
    return descriptorSetLayout.recordAndReturnSets();
}

std::vector<vk::DescriptorSet> TemporalReproject::writeDescriptorSetAddData(int i) {
    DescriptorSetLayout& descriptorSetLayout = _computeAddData[i]->getDecriptorSetLayaut();
    descriptorSetLayout.allocateSets(1);
    descriptorSetLayout.beginSet(0);
    descriptorSetLayout.bindCombinedImageSampler(0, _sourceColorTexture.getView(), _sourceSampler);
    descriptorSetLayout.bindCombinedImageSampler(1, _sourceDepthTexture.getView(), _sourceSampler);
    descriptorSetLayout.bindStorageImage(2, _targetColorTextures[i].getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindStorageImage(3, _targetDepthTextures[i].getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindUniformBuffer(4, _shiftBuffer.getBuf(), 0, sizeof(UBOJitterShift));
    return descriptorSetLayout.recordAndReturnSets();
}

std::vector<vk::DescriptorSet> TemporalReproject::writeDescriptorSetResolve(int i) {
    DescriptorSetLayout& descriptorSetLayout = _computeResolve[i]->getDecriptorSetLayaut();
    descriptorSetLayout.allocateSets(1);
    descriptorSetLayout.beginSet(0);
    descriptorSetLayout.bindStorageImage(0, _targetColorTextures[i].getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindStorageImage(1, _targetResolveTexture.getView(), vk::ImageLayout::eGeneral);
    return descriptorSetLayout.recordAndReturnSets();
}

void TemporalReproject::recordCmdBuffers() {
    std::vector<vk::DescriptorSet> descriptorSet;
    for (int i = 0; i < 2; i++) {
        descriptorSet = writeDescriptorSetReproject(i);
        vk::CommandBuffer commandBufferReproject = _computeReproject[i]->getCommandBuffer().begin();
        {
            commandBufferReproject.bindPipeline(vk::PipelineBindPoint::eCompute, _computeReproject[i]->getPipeline());
            commandBufferReproject.bindDescriptorSets(vk::PipelineBindPoint::eCompute, 
                _computeReproject[i]->getPipelineLayout(), 0, descriptorSet, nullptr);

            commandBufferReproject.dispatch((_targetExtent.width + 15) / 16, (_targetExtent.height + 15) / 16, 1);
        }
        _computeReproject[i]->getCommandBuffer().end();

        descriptorSet = writeDescriptorSetAddData(i);
        vk::CommandBuffer commandBufferAddData = _computeAddData[i]->getCommandBuffer().begin();
        {
            commandBufferAddData.bindPipeline(vk::PipelineBindPoint::eCompute, _computeAddData[i]->getPipeline());
            commandBufferAddData.bindDescriptorSets(vk::PipelineBindPoint::eCompute, 
                _computeAddData[i]->getPipelineLayout(), 0, descriptorSet, nullptr);

            commandBufferAddData.dispatch((_sourceExtent.width + 15) / 16, (_sourceExtent.height + 15) / 16, 1);
        }
        _computeAddData[i]->getCommandBuffer().end();

        descriptorSet = writeDescriptorSetResolve(i);
        vk::CommandBuffer commandBufferResolve = _computeResolve[i]->getCommandBuffer().begin();
        {
            commandBufferResolve.bindPipeline(vk::PipelineBindPoint::eCompute, _computeResolve[i]->getPipeline());
            commandBufferResolve.bindDescriptorSets(vk::PipelineBindPoint::eCompute, 
                _computeResolve[i]->getPipelineLayout(), 0, descriptorSet, nullptr);

            commandBufferResolve.dispatch((_sourceExtent.width + 15) / 16, (_sourceExtent.height + 15) / 16, 1);
        }
        _computeResolve[i]->getCommandBuffer().end();
    }
}

CmdSync TemporalReproject::reproject(
        const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences
) {
    CmdSync _syncReproject = _computeReproject[_targetIndex]->compute(waitSemaphores, waitFences);
    CmdSync _syncAddData = _computeAddData[_targetIndex]->compute({ _syncReproject.getSemaphore() }, {});
    CmdSync _syncResolve = _computeResolve[_targetIndex]->compute({ _syncAddData.getSemaphore() }, {});
    _targetIndex = (_targetIndex + 1) % 2;
    return _syncResolve;
}

void TemporalReproject::createUniformBuffers() {
    _transformBuffer = _device.getBufferManager().createUniformBuffer("temporalReproject.uboTransform", sizeof(UBOTransformMatrix));
    _shiftBuffer = _device.getBufferManager().createUniformBuffer("temporalReproject.uboShift", sizeof(UBOJitterShift));
    updateUniformBuffers(glm::mat4(1.0), 0);
}

void TemporalReproject::updateUniformBuffers(glm::mat4 view, int jitterInd) {
    _uboTransform.transform = _proj * view * _prevInverseView * _inverseProj;
    _uboShift.shift = _jitterShift[jitterInd];
    BufferManager &bufferManager = _device.getBufferManager();
    bufferManager.copyDataToBuffer(_transformBuffer, &_uboTransform, sizeof(UBOTransformMatrix));
    bufferManager.copyDataToBuffer(_shiftBuffer, &_uboShift, sizeof(UBOJitterShift));
    // vk::Device device = _device.getDevice();
    // auto[result, data_ptr] = device.mapMemory(_uniformBuffer.getMem(), 0, sizeof(UBOTransformMatrix));
    // memcpy(data_ptr, &_uboTransform, sizeof(UBOTransformMatrix));
    // device.unmapMemory(_uniformBuffer.getMem());
    _prevInverseView = computeInverseView(view);
}

glm::mat4 TemporalReproject::computeInverseProj(const glm::mat4 proj) {
    glm::mat4 inverse_proj(0);
    inverse_proj[0][0] = 1 / proj[0][0];
    inverse_proj[1][1] = 1 / proj[1][1];
    inverse_proj[2][3] = 1 / proj[3][2];
    inverse_proj[3][2] = 1 / proj[2][3];
    inverse_proj[3][3] = - proj[2][2] / (proj[2][3] * proj[3][2]);
    return inverse_proj;
}

glm::mat4 TemporalReproject::computeInverseView(const glm::mat4 view) {
    glm::mat3 inverse_A(0);
    inverse_A[0][0] = view[0][0];
    inverse_A[0][1] = view[1][0];
    inverse_A[0][2] = view[2][0];
    inverse_A[1][0] = view[0][1];
    inverse_A[1][1] = view[1][1];
    inverse_A[1][2] = view[2][1];
    inverse_A[2][0] = view[0][2];
    inverse_A[2][1] = view[1][2];
    inverse_A[2][2] = view[2][2];


    glm::vec3 b(view[3][0], view[3][1], view[3][2]);
    glm::vec3 inverse_b = -inverse_A * b;
    glm::mat4 inverse_view(1);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            inverse_view[i][j] = inverse_A[i][j];
        }
        inverse_view[3][i] = inverse_b[i];
    }
    return inverse_view;
}