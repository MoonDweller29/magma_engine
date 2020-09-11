#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 globalPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 1) uniform sampler2D texSampler;
layout(binding = 2) uniform UniformBufferObject {
    vec3 cameraPos;
    vec3 lightPos;
} ubo;

vec3 occlusion_color = vec3(0.1, 0.1, 0.15);
//vec3 lightDir = normalize(vec3(0.8, -1, 0.9));
vec3 lightColor = vec3(0.7,0.7,0.7);
float lightIntencity = 2.5f;

void main() {
//    outColor = vec4(vNormal, 1.0);
//    outColor = vec4(vTexCoord, 0.0, 1.0);
    vec3 lightDir = vec3(0.0) - ubo.lightPos;

    vec3 viewVec = normalize(globalPos - ubo.cameraPos);
    float diffuse = clamp(dot(lightDir, -vNormal), 0.0f, 1.0f);
    float specular = pow(
        clamp(
            dot(normalize(lightDir + viewVec), -normalize(vNormal))*sign(diffuse), 0.0f, 1.0f
        ), 10.0f
    );
    float sqrLightPathLen = dot(ubo.lightPos - globalPos, ubo.lightPos - globalPos);
    float lightSaturation = clamp(lightIntencity * (diffuse*0.6f + specular*0.3f) / sqrLightPathLen, 0.0f, 1.0f);
    outColor = vec4(lightColor*(lightSaturation) + occlusion_color, 1.0f);
    outColor.rgb *= texture(texSampler, vTexCoord).rgb;
}