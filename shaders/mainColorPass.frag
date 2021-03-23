#version 450

layout(location = 0) out vec4 outAlbedo;
layout(location = 1) out vec4 outNormal;
layout(location = 2) out vec4 outGlobalPos;

layout(location = 0) in vec3 inGlobalPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(binding = 1) uniform sampler2D albedoTex;


void main() {
    outAlbedo    = texture(albedoTex, inTexCoord);
    outNormal    = vec4(normalize(inNormal), 1.0f);
    outGlobalPos = vec4(inGlobalPos, 1.0f);
}