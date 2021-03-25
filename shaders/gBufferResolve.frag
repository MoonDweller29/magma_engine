#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in  vec2 inUV;

layout(binding = 0) uniform sampler2D albedoTex;
layout(binding = 1) uniform sampler2D normalsTex;
layout(binding = 2) uniform sampler2D worldPosTex;
layout(binding = 3) uniform sampler2D shadowMap;

layout(binding = 4) uniform UniformBufferObject {
    vec3 cameraPos;
    vec3 lightPos;
} ubo;

layout(binding = 5) uniform LightSpaceUniform {
    mat4 viewProj;
} lubo;

void main() {
    vec3 albedo   = texture(albedoTex, inUV).rgb;
    vec3 normal   = normalize(texture(normalsTex, inUV).rgb);
    vec3 worldPos = texture(worldPosTex, inUV).rgb;
    
    //@TODO: add info about light source
    outColor = vec4(albedo, 1.0f);
}