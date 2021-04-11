/**
 * @file motionVector.cpp
 * @author Nastormo
 * @brief Calculate motion vector
 * @version 0.1
 * @date 2021-01-30
 *
 * @copyright Copyright (c) 2021
 */
#include "magma/app/postProcess/MotionVector.h"
#include "magma/app/log.hpp"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/descriptors/DescriptorSetLayoutInfo.h"
#include "magma/vk/textures/TextureManager.h"
#include <memory>
#include <tuple>
#include <vulkan/vulkan.hpp>

/**
 * @brief Construct a new Motion Vector:: Motion Vector object
 *
 * @param device logical device which compute motion vector
 */
MotionVector::MotionVector(LogicalDevice &device, const Texture &textureSource, glm::mat4 proj)
        : _device(device),
        _textureSource(textureSource),
        _textureExtent(toExtent2D(_textureSource.getInfo()->imageInfo.extent)),
        _proj(proj),
        _inverseProj(computeInverseProj(proj)) {
    createUniformBuffer();
    createSourceSampler();
    createTextureTarget();

    DescriptorSetLayoutInfo computeLayoutInfo;
    computeLayoutInfo.addCombinedImageSampler(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfo.addStorageImage(vk::ShaderStageFlagBits::eCompute);
    computeLayoutInfo.addUniformBuffer(1, vk::ShaderStageFlagBits::eCompute);

    Shader computeShader("motionVector.comp", _device.getDevice(), "shaders/motionVector.comp.spv", Shader::Stage::COMP_SH);

    _compute = std::make_unique<ComputePass>(device, computeLayoutInfo, computeShader);
}

/**
 * @brief Destroy the Motion Vector:: Motion Vector object
 */
MotionVector::~MotionVector() {
    _device.getBufferManager().deleteBuffer(_mvUniform);
    _device.getDevice().destroySampler(_sourceSampler);
    _device.getTextureManager().deleteTexture(_textureTarget);
}

/**
 * @brief create uniform buffer which contain camera rotation
 */
void MotionVector::createUniformBuffer() {
    _mvUniform = _device.getBufferManager().createUniformBuffer("motionVector.ubo", sizeof(UBOMotionVector));
    updateUniformBuffer(glm::mat4(1.0));
}

/**
 * @brief create target texture which contain motion vector
 *
 * @param width width target texture
 * @param height height tagret texture
 */
void MotionVector::createTextureTarget() {
    TextureManager & textureMenager = _device.getTextureManager();
    _textureTarget = textureMenager.createTexture2D("motionVector.tex", _textureTargetFormat, _textureExtent, 
        vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eStorage, vk::ImageAspectFlagBits::eColor);
    textureMenager.setLayout(_textureTarget, vk::ImageLayout::eGeneral);
}

void MotionVector::createSourceSampler()
{
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

std::vector<vk::DescriptorSet> MotionVector::writeDescriptorSet() {
    DescriptorSetLayout& descriptorSetLayout = _compute->getDecriptorSetLayaut();
    descriptorSetLayout.allocateSets(1);
    descriptorSetLayout.beginSet(0);
    descriptorSetLayout.bindCombinedImageSampler(0, _textureSource.getView(), _sourceSampler);
    descriptorSetLayout.bindStorageImage(1, _textureTarget.getView(), vk::ImageLayout::eGeneral);
    descriptorSetLayout.bindUniformBuffer(2, _mvUniform.getBuf(), 0, sizeof(UBOMotionVector));
    return descriptorSetLayout.recordAndReturnSets();
}

void MotionVector::recordCmdBuffers() {
    std::vector<vk::DescriptorSet> descriptorSet = writeDescriptorSet();
    vk::CommandBuffer commandBuffer = _compute->getCommandBuffer().begin();
    {
        commandBuffer.bindPipeline(vk::PipelineBindPoint::eCompute, _compute->getPipeline());
        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eCompute, 
            _compute->getPipelineLayout(), 0, descriptorSet, nullptr);

        commandBuffer.dispatch((_textureExtent.width + 15) / 16, (_textureExtent.height + 15) / 16, 1);
    }
     _compute->getCommandBuffer().end();
}

/**
 * @brief compute motion vector
 *
 * @return CmdSync Execution dependency between compute & graphic submission
 */
CmdSync MotionVector::computeMotionVector(
        const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences
) {
    return _compute->compute(waitSemaphores, waitFences);
}

/**
 * @brief update camera uniform
 * @todo will need add input camera param
 */
void MotionVector::updateUniformBuffer(glm::mat4 view) {
    _uboMV.transform = _proj * view * _prevInverseView * _inverseProj;
    vk::Device device = _device.getDevice();
    auto[result, data_ptr] = device.mapMemory(_mvUniform.getMem(), 0, sizeof(UBOMotionVector));
    memcpy(data_ptr, &_uboMV, sizeof(UBOMotionVector));
    device.unmapMemory(_mvUniform.getMem());
    _prevInverseView = computeInverseView(view);
}

/*
Projection matrix
a 0 0 0
0 b 0 0
0 0 c d
0 0 e 0

Inverse projection
1/a, 0, 0, 0,
0, 1/b, 0, 0,
0, 0, 0, 1/e,
0, 0, 1/d, -c/(d*e)
*/
glm::mat4 MotionVector::computeInverseProj(const glm::mat4 proj) {
    glm::mat4 inverse_proj(0);
    inverse_proj[0][0] = 1 / proj[0][0];
    inverse_proj[1][1] = 1 / proj[1][1];
    inverse_proj[2][3] = 1 / proj[3][2];
    inverse_proj[3][2] = 1 / proj[2][3];
    inverse_proj[3][3] = - proj[2][2] / (proj[2][3] * proj[3][2]);
    return inverse_proj;
}

/*
View matrix
|  A   b|
|0...0 1|

Inverse view
|A^{-1} -A^{-1}b|
|0...0      1   |

A
|a00 a01 a02|
|a10 a11 a12|
|a20 a21 a22|

det A
a00 * (a11 * a22 - a21 * a12) -
    a01 * (a10 * a22 - a12 * a20) +
    a02 * (a10 * a21 - a11 * a20)
*/
// glm::mat4 MotionVector::computeInverseView(const glm::mat4 view) {
//     float det_A00 = view[1][1] * view[2][2] - view[2][1] * view[1][2];
//     float det_A01 = view[1][0] * view[2][2] - view[1][2] * view[2][0];
//     float det_A02 = view[1][0] * view[2][1] - view[1][1] * view[2][0];
//     float det_A = view[0][0] * det_A00 - view[0][1] * det_A01 + view[0][2] * det_A02;
//     glm::mat3 inverse_A(0);
//     inverse_A[0][0] = det_A00 / det_A;
//     inverse_A[0][1] = (view[0][2] * view[2][1] - view[0][1] * view[2][2]) / det_A;
//     inverse_A[0][2] = (view[0][1] * view[1][2] - view[0][2] * view[1][1]) / det_A;
//     inverse_A[1][0] = -det_A01 / det_A;
//     inverse_A[1][1] = (view[0][0] * view[2][2] - view[0][2] * view[2][0]) / det_A;
//     inverse_A[1][2] = (view[1][0] * view[0][2] - view[0][0] * view[1][2]) / det_A;
//     inverse_A[2][0] = det_A02 / det_A;
//     inverse_A[2][1] = (view[2][0] * view[0][1] - view[0][0] * view[2][1]) / det_A;
//     inverse_A[2][2] = (view[0][0] * view[1][1] - view[1][0] * view[0][1]) / det_A;


//     glm::vec3 b(view[3][0], view[3][1], view[3][2]);
//     glm::vec3 inverse_b = -inverse_A * b;
//     glm::mat4 inverse_view(1);
//     for (int i = 0; i < 3; i++) {
//         for (int j = 0; j < 3; j++) {
//             inverse_view[i][j] = inverse_A[i][j];
//         }
//         inverse_view[3][i] = inverse_b[i];
//     }
//     return inverse_view;
// }

glm::mat4 MotionVector::computeInverseView(const glm::mat4 view) {
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

