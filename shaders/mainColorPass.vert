#version 450

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 outGlobalPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outTexCoord;

layout(binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

void main() {
    outGlobalPos = (ubo.model * vec4(inPosition, 1.0)).xyz;
    gl_Position = ubo.proj * ubo.view * vec4(outGlobalPos, 1.0);

    outNormal   = inNormal;
    outTexCoord = inTexCoord;
}