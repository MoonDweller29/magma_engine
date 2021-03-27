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

float shadowCalc(vec3 worldPos, vec3 normal, vec3 lightDir) {
    //calculating light space position
    vec4 lightSpacePos = lubo.viewProj * vec4(worldPos, 1.0f);
    vec3 projCoords = lightSpacePos.xyz / lightSpacePos.w;

    float currentDepth = projCoords.z;
    projCoords.xy = projCoords.xy * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r;

    float bias = clamp(0.05 * (1.0 - dot(normal, lightDir)), 0, 0.002);
//    float bias = 0.001;
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    return shadow;
}

vec3 ambientColor = vec3(0.1, 0.1, 0.15);
vec3 lightColor = vec3(0.7,0.7,0.7);
float lightIntencity = 2.5f;

void main() {
    vec3 albedo   = texture(albedoTex, inUV).rgb;
    vec3 normal   = normalize(texture(normalsTex, inUV).rgb);
    vec3 worldPos = texture(worldPosTex, inUV).rgb;
    
    vec3 lightDir = normalize(-ubo.lightPos);
    vec3 viewVec  = normalize(worldPos - ubo.cameraPos);

    float diffuse = clamp(dot(lightDir, -normal), 0.0f, 1.0f);
    float specular = pow(
        clamp(
            dot(normalize(lightDir + viewVec), -normal)*sign(diffuse), 0.0f, 1.0f
        ), 10.0f
    );

    float lightSaturation = clamp(lightIntencity * (diffuse*0.6f + specular*0.3f), 0.0f, 1.0f);
    float shadow = shadowCalc(worldPos, normal, lightDir);

    outColor = vec4(lightColor*(lightSaturation)*(1 - shadow) + ambientColor, 1.0f);
    outColor.rgb *= albedo;
}