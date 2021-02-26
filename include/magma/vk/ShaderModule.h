#pragma once
#include <vulkan/vulkan.hpp>
#include <string>
#include <vector>

class Shader {
public:
    enum class Stage {
        VERT_SH,
        GEOM_SH,
        FRAG_SH,
        COMP_SH
    };

    Shader(const std::string &name, vk::Device device, const std::string &path, Stage stage);
    ~Shader();

    vk::ShaderModule                    getModule()     { return _shaderModule; }
    vk::PipelineShaderStageCreateInfo   getStageInfo()  { return _stageInfo;    }

    static vk::ShaderStageFlagBits stageToVkStage(Stage stage);

private:
    vk::Device _device;
    vk::ShaderModule _shaderModule;
    vk::PipelineShaderStageCreateInfo _stageInfo;
};
