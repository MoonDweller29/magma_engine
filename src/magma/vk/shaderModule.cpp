#include "magma/vk/shaderModule.h"
#include "magma/vk/vulkan_common.h"
#include <fstream>

Shader::Shader(VkDevice device, const std::string &name, Stage stage)
{
    auto shaderCode = readFile(name);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    VK_CHECK_ERR(result, "failed to create shader module!");
    this->device = device;

    //vertShaderStageInfo creation
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = stageToVkStage(stage);
    stageInfo.module = shaderModule;
    stageInfo.pName = "main";
    stageInfo.pSpecializationInfo  = nullptr; //can change constants in shader
}

VkShaderStageFlagBits Shader::stageToVkStage(Stage stage)
{
    switch (stage)
    {
        case VERT_SH:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case GEOM_SH:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case FRAG_SH:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case COMP_SH:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        default:
            throw std::runtime_error("WRONG SHADER STAGE");
    }
}

std::vector<char> Shader::readFile(const std::string &filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open())
    {
        throw std::runtime_error("failed to open file!");
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);
    file.seekg(0); //move cursor to the beginning
    file.read(buffer.data(), fileSize); //actual reading

    file.close();

    return buffer;
}


Shader::~Shader()
{
    vkDestroyShaderModule(device, shaderModule, nullptr);
}
