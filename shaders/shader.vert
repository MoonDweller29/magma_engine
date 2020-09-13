#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(binding = 3) uniform LightSpaceUniform {
    mat4 viewProj;
} lubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 globalPos;
layout(location = 1) out vec3 vNormal;
layout(location = 2) out vec2 vTexCoord;
layout(location = 3) out vec4 lightspacePos;

void main()
{
    globalPos = (ubo.model * vec4(inPosition, 1.0)).xyz;
    gl_Position = ubo.proj * ubo.view * vec4(globalPos, 1.0);
    lightspacePos = lubo.viewProj * vec4(globalPos, 1.0);

    vNormal = inNormal;
    vTexCoord = inTexCoord;
}