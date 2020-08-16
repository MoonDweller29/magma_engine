#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>


class Shader
{
public:
    enum Stage
    {
        VERT_SH,
        GEOM_SH,
        FRAG_SH
    };
private:
    VkShaderModule shaderModule;
    VkPipelineShaderStageCreateInfo stageInfo = {};
    VkDevice device;
    static VkShaderStageFlagBits stageToVkStage(Stage stage);
public:
    static std::vector<char> readFile(const std::string &filename);
    Shader(VkDevice device, const std::string &name, Stage stage);
    VkShaderModule getModule() { return shaderModule; }
    VkPipelineShaderStageCreateInfo getStageInfo() { return stageInfo; }
    ~Shader();
};
