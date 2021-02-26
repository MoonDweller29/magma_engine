#include "magma/vk/ShaderModule.h"

#include <fstream>

#include "magma/app/log.hpp"
#include "magma/vk/vulkan_common.h"

static std::vector<char> readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        LOG_AND_THROW std::runtime_error("Failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

static vk::ShaderStageFlagBits stageToVkStage(ShaderStage stage) {
    switch (stage)
    {
        case VERT_SH:
            return vk::ShaderStageFlagBits::eVertex;
        case GEOM_SH:
            return vk::ShaderStageFlagBits::eGeometry;
        case FRAG_SH:
            return vk::ShaderStageFlagBits::eFragment;
        case COMP_SH:
            return vk::ShaderStageFlagBits::eCompute;
        default:
            LOG_AND_THROW std::runtime_error("Unsupported shader stage");
    }
}

/// @todo add constant value
Shader::Shader(const std::string &name, vk::Device device, const std::string &path, ShaderStage stage) 
        : _device(device) {
    auto shaderCode = readFile(path);

    vk::ShaderModuleCreateInfo createInfo;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    vk::Result result;
    std::tie(result, _shaderModule) = _device.createShaderModule(createInfo);
    VK_HPP_CHECK_ERR(result, "Failed to create shader module!");

    _stageInfo.stage = stageToVkStage(stage);
    _stageInfo.module = _shaderModule;
    _stageInfo.pName = "main";
    //_stageInfo.pSpecializationInfo - needed for constants value
}

Shader::~Shader() {
    _device.destroyShaderModule(_shaderModule);
}
