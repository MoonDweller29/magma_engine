#pragma once
#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>

enum ShaderStage {
    VERT_SH,
    GEOM_SH,
    FRAG_SH,
    COMP_SH
};

class Shader {
public:
    Shader(const std::string &name, vk::Device device, const std::string &path, ShaderStage stage);
    ~Shader();

    vk::ShaderModule                    getModule()     { return _shaderModule; }
    vk::PipelineShaderStageCreateInfo   getStageInfo()  { return _stageInfo;    }

private:
    vk::Device _device;
    vk::ShaderModule _shaderModule;
    vk::PipelineShaderStageCreateInfo _stageInfo;
};
