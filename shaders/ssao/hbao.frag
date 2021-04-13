#version 450

layout(location = 0) out vec4 outAO;

layout(location = 0) in  vec2 inUV;

layout(binding = 0) uniform sampler2D depthTex;
layout(binding = 1) uniform sampler2D normalsTex;
layout(binding = 2) uniform sampler2D blueNoiseTex; //size = 32x32
layout(binding = 3) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
layout(binding = 4) uniform InverseProjUniform {
    vec2 screenSize;
    vec2 zNearFar;
    mat4 invProj;
    mat4 invView;
} invProjUBO;

// sampling directions for 1st sector
#define DIR_COUNT 16
#define PI 3.14159265
vec2 const_dirs[DIR_COUNT] = {
	vec2(1.0000, 0.0000),
	vec2(0.9239, 0.3827),
	vec2(0.7071, 0.7071),
	vec2(0.3827, 0.9239),
	vec2(0.0000, 1.0000),
	vec2(-0.3827, 0.9239),
	vec2(-0.7071, 0.7071),
	vec2(-0.9239, 0.3827),
	vec2(-1.0000, 0.0000),
	vec2(-0.9239, -0.3827),
	vec2(-0.7071, -0.7071),
	vec2(-0.3827, -0.9239),
	vec2(-0.0000, -1.0000),
	vec2(0.3827, -0.9239),
	vec2(0.7071, -0.7071),
	vec2(0.9239, -0.3827)
};
float stepSize = 2.0f/960.f;
int stepCount = 10;
float R = 0.2f; // influence radius

mat2 rotMat(float angle) {
	float cos_a = cos(angle);
	float sin_a = sin(angle);
	return mat2(
		cos_a, sin_a, //first  column
		-sin_a, cos_a //second column
	);
}

float linearizeDepth(float depth) {
	float n = invProjUBO.zNearFar.x;
	float f = invProjUBO.zNearFar.y;

	return n*f/(f - depth*(f - n));
}

float attenuationFunc(float r) {
	return max(0, 1 - r/R);
}

vec4 toViewSpace(vec4 pos, float z) {
	return invProjUBO.invProj * pos * z;
}

float hbaoInDirection(vec2 dir, vec3 pos, vec3 normal, float stepSize) {
	vec2 xy = inUV*2.0 - 1.0;
	float r = R;
	float sinH = 0; //sin(h(theta)) - sin of elevation angle

	for (int i = 0; i < stepCount; ++i) {
		vec2 step = dir*stepSize*i;
		float depth = texture(depthTex, inUV+step).r;
		float z = linearizeDepth(depth);
		vec3 currPos = toViewSpace(vec4(xy+step*2, depth, 1.0f), z).xyz;
		vec3 origToSample = currPos - pos;
		float currR = sqrt(dot(origToSample, origToSample));
		origToSample /= currR;
		float currSinH = dot(normal, origToSample);
		if (currSinH > sinH) {
			r = currR;
			sinH = currSinH;
		}
	}


	return sinH * attenuationFunc(r);
}

void main() {
	vec3 normal = normalize((ubo.view * vec4(texture(normalsTex, inUV).xyz, 0.0)).xyz); //in view space
	float depth = texture(depthTex, inUV).r;
	float z = linearizeDepth(depth);
	vec4 pos = vec4(inUV*2.0 - 1.0, depth, 1.0f);
	pos = toViewSpace(pos, z);


	int dirCount = DIR_COUNT;

	ivec2 screenCoord = ivec2(inUV*invProjUBO.screenSize);
	vec2 noiseCoord = vec2(screenCoord/32.0f);
	float rotAngle = texture(blueNoiseTex, noiseCoord).r * 2*PI/dirCount;
	mat2 dirRotMat = rotMat(rotAngle);

	vec2 dirs[DIR_COUNT];
	for (int i = 0; i < dirCount; ++i) {
		dirs[i] = dirRotMat * const_dirs[i];
	}

	float ao = 0.0f;
	for (int i = 0; i < dirCount; ++i) {
		ao += hbaoInDirection(dirs[i], pos.xyz+0.001*normal, normal, stepSize);
	}
	ao = 1 - ao/dirCount;



//	outAO = linearizeDepth(depth).rrr;
//	outAO = normal;
//	outAO = vec4(pos.xyz, 1.0f);
	outAO = vec4(ao.rrr, 1.0f);
//	outAO = vec4(texture(blueNoiseTex, noiseCoord).rrr, 1.0f);
}