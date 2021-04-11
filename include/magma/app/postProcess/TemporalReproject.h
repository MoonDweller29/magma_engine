#pragma once

#include "glm/fwd.hpp"
#include "magma/glm_inc.h"
#include "magma/vk/LogicalDevice.h"
#include "magma/vk/textures/Texture.h"
#include "magma/vk/textures/TextureManager.h"
#include "magma/app/compute/ComputePass.h"
#include <vulkan/vulkan.hpp>

class TemporalReproject {
public:
    TemporalReproject(LogicalDevice &device, const Texture &colorTexture, const Texture &mvTexture, glm::mat4 proj);
    ~TemporalReproject();

    Texture& getResolve() { return _targetResolveTexture; }

    void updateUniformBuffers(glm::mat4 view, int jitterInd);
    void recordCmdBuffers();
    CmdSync reproject(const std::vector<vk::Semaphore> &waitSemaphores,
        const std::vector<vk::Fence> &waitFences);

private:
    struct UBOTransformMatrix {
        glm::mat4 transform;
    } _uboTransform;

    struct UBOJitterShift {
        glm::ivec2 shift;
    } _uboShift;

    LogicalDevice &_device;

    vk::Extent2D    _sourceExtent;
    vk::Sampler     _sourceSampler;
    Texture         _sourceDepthTexture;
    Texture         _sourceColorTexture;

    int                                         _targetIndex;
    vk::Extent2D                                _targetExtent;
    vk::Format                                  _targetColorTextureFormat = vk::Format::eR16G16B16A16Unorm;
    std::vector<Texture>                        _targetColorTextures;
    vk::Format                                  _targetDepthTextureFormat = vk::Format::eR16Unorm;
    std::vector<Texture>                        _targetDepthTextures;
    vk::Format                                  _targetResolveTextureFormat = vk::Format::eR16G16B16A16Unorm;
    Texture                                     _targetResolveTexture;

    glm::mat4                       _proj;
    glm::mat4                       _inverseProj;
    glm::mat4                       _prevInverseView;
    Buffer                          _transformBuffer;

    std::vector<glm::ivec2>         _jitterShift;
    Buffer                          _shiftBuffer;

    vk::Format  _finalTextureFormat = vk::Format::eR16G16B16A16Unorm;
    Texture     _finalTexture;

    std::vector<std::unique_ptr<ComputePass>>   _computeReproject;
    std::vector<std::unique_ptr<ComputePass>>   _computeAddData;
    std::vector<std::unique_ptr<ComputePass>>   _computeResolve;

    void createUniformBuffers();
    void createSourceSampler();
    void createTargetTextures();
    std::vector<vk::DescriptorSet> writeDescriptorSetReproject(int i);
    std::vector<vk::DescriptorSet> writeDescriptorSetAddData(int i);
    std::vector<vk::DescriptorSet> writeDescriptorSetResolve(int i);
    glm::mat4 computeInverseView(const glm::mat4 view);
    glm::mat4 computeInverseProj(const glm::mat4 proj);
};