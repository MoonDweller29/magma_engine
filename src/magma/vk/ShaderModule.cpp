#include "magma/vk/ShaderModule.h"

#include <fstream>

#include "magma/app/log.hpp"
#include "magma/vk/vulkan_common.h"

static std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        LOG_AND_THROW std::runtime_error("Failed to open file " + filename);
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0); //move cursor to the beginning
    file.read(buffer.data(), fileSize); //actual reading

    file.close();

    return buffer;
}

/// @todo add constant value
Shader::Shader(const std::string &name, vk::Device device, const std::string &path, Stage stage) 
        : _device(device) {
    auto shaderCode = readFile(path);

    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    vk::Result result;
    std::tie(result, _shaderModule) = _device.createShaderModule(createInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create " + name + " shader module!");

    _stageInfo.stage = stageToVkStage(stage);
    _stageInfo.module = _shaderModule;
    _stageInfo.pName = "main";
    //_stageInfo.pSpecializationInfo - needed for constants value
}

Shader::~Shader() {
    _device.destroyShaderModule(_shaderModule);
}

vk::ShaderStageFlagBits Shader::stageToVkStage(Shader::Stage stage) {
    switch (stage) {
        case Shader::Stage::VERT_SH:
            return vk::ShaderStageFlagBits::eVertex;
        case Shader::Stage::GEOM_SH:
            return vk::ShaderStageFlagBits::eGeometry;
        case Shader::Stage::FRAG_SH:
            return vk::ShaderStageFlagBits::eFragment;
        case Shader::Stage::COMP_SH:
            return vk::ShaderStageFlagBits::eCompute;
        default:
            LOG_AND_THROW std::runtime_error("Unsupported shader stage");
    }
}
