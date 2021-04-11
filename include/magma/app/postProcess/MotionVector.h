/**
 * @file motionVector.h
 * @author Nastormo
 * @brief Calculate motion vector
 * @version 0.1
 * @date 2021-01-30
 *
 * @copyright Copyright (c) 2021
 */
#pragma once
#include <vulkan/vulkan.hpp>
#include <iostream>

#include "magma/glm_inc.h"

#include "magma/vk/ShaderModule.h"
#include "magma/vk/CmdSync.h"
#include "magma/vk/descriptors/DescriptorSetLayout.h"
#include "magma/vk/pipeline/ComputePipeline.h"
#include "magma/vk/commands/CommandBuffer.h"
#include "magma/vk/textures/TextureManager.h"
#include "magma/app/compute/ComputePass.h"

class MotionVector {
public:
    MotionVector(LogicalDevice &device, const Texture &textureSource, glm::mat4 proj);
    ~MotionVector();

    Texture& getMotionVector() { return _textureTarget; }

    void updateUniformBuffer(glm::mat4 view);
    void recordCmdBuffers();
    CmdSync computeMotionVector(const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences);

private:
    struct UBOMotionVector {
        glm::mat4 transform;
    } _uboMV;

    LogicalDevice                 & _device;

    Texture                         _textureSource;
    vk::Extent2D                    _textureExtent;
    vk::Sampler                     _sourceSampler;

    vk::Format                      _textureTargetFormat = vk::Format::eR16G16B16A16Unorm;
    Texture                         _textureTarget;

    glm::mat4                       _proj;
    glm::mat4                       _inverseProj;
    glm::mat4                       _prevInverseView;
    Buffer                          _mvUniform;

    std::unique_ptr<ComputePass>    _compute;

    void createUniformBuffer();
    void createSourceSampler();
    void createTextureTarget();
    std::vector<vk::DescriptorSet> writeDescriptorSet();
    glm::mat4 computeInverseView(const glm::mat4 view);
    glm::mat4 computeInverseProj(const glm::mat4 proj);
};