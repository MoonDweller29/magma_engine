#version 450

layout(location = 0) out vec4 outAO;

layout(location = 0) in  vec2 inUV;

layout(binding = 0) uniform sampler2D depthTex;
layout(binding = 1) uniform sampler2D normalsTex;
layout(binding = 2) uniform sampler2D blueNoiseTex; //size = 32x32
layout(binding = 3) uniform sampler2D depthPyramid[5];
layout(binding = 4) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;
layout(binding = 5) uniform InverseProjUniform {
    vec2 screenSize;
    vec2 zNearFar;
    mat4 invProj;
    mat4 invView;
} invProjUBO;

#define DIR_COUNT 8
#define PI 3.14159265
vec2 const_dirs[DIR_COUNT] = {
//	vec2(1.0000, 0.0000),
//	vec2(0.7071, 0.7071),
//	vec2(0.0000, 1.0000),
//	vec2(-0.7071, 0.7071),
//	vec2(-1.0000, 0.0000),
//	vec2(-0.7071, -0.7071),
//	vec2(-0.0000, -1.0000),
//	vec2(0.7071, -0.7071)
//vec2(-1.0976, 0.5792),
//vec2(-0.5560, -0.8715),
//vec2(-1.7714, 0.9439),
//vec2(1.3436, -0.8383),
//vec2(1.4376, -1.3320),
//vec2(-1.2052, -1.8677),
//vec2(-1.0597, 2.2327),
//vec2(-1.0702, 0.0922),
vec2(-1.4371, -0.5543),
vec2(1.7244, 1.1745),
vec2(-2.2617, -0.4307),
vec2(0.9772, 0.8003),
vec2(-1.3958, 1.1923),
vec2(-1.6989, -0.9874),
vec2(1.1792, 0.0024),
vec2(-0.5500, 1.9295)
};

int stepCount = 5;
float R = 0.7f; // influence radius

float max4(vec4 vec) {
	return max(vec.x, max(vec.y, max(vec.z, vec.w)));
}

float min4(vec4 vec) {
	return min(vec.x, min(vec.y, min(vec.z, vec.w)));
}

vec3 customSampler(sampler2D texSampler, vec2 uv) {
	ivec2 texSize = textureSize(texSampler, 0);
	vec2 coef = fract(uv * texSize - vec2(0.499f));
	vec4 tex4 = textureGather(texSampler, uv, 1);
	vec2 interp = mix(tex4.wz, tex4.xy, coef.y);
	vec3 res;
	res.g = mix(interp.x, interp.y, coef.x);
	tex4 = textureGather(texSampler, uv, 0);
	res.r = min4(tex4);
	tex4 = textureGather(texSampler, uv, 2);
	res.b = max4(tex4);

	return res;
}

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

ivec2 toIntCoords(vec2 coords, ivec2 screenSize) {
	return ivec2(coords * screenSize - 0.45f);
}

vec2 toFloatCoords(ivec2 coords, vec2 pixelSize) {
	return (coords + 0.5f) * pixelSize;
}

float attenuationFunc(float r) {
	return max(0, 1 - r/R);
}

vec4 toViewSpace(vec4 pos, float z) {
	return invProjUBO.invProj * pos * z;
}

int intPow(int x, int power) {
	int res = x;
	for (int i = 0; i < power-1; ++i) {
		res *= x;
	}

	return x;
}

float pyramidAoInDirection(vec2 dir, vec3 pos, vec3 normal, vec2 stepSize) {
	vec2 xy = inUV*2.0 - 1.0;

	vec2 step = dir*stepSize;
	float depth = texture(depthTex, inUV+step).r;
	float z = linearizeDepth(depth);
	vec3 currPos = toViewSpace(vec4(xy+step*2, depth, 1.0f), z).xyz;
	vec3 origToSample = currPos - pos;
	float r = sqrt(dot(origToSample, origToSample));
	origToSample /= r;
	float sinH = dot(normal, origToSample);

	float ao = max(0, sinH * attenuationFunc(r) * 0.1);
	ao = 0;
	sinH = 0;

	vec2 pixelSize = 1.0f / invProjUBO.screenSize;
	vec2 screenPos = inUV * invProjUBO.screenSize;
	int n = 1;
	int subsamples = 2;
	for (int i = 0; i < stepCount; ++i) {
		n *= 3;
		for (int j = 0; j < subsamples; ++j) {
//			mat2 rotMatJ = rotMat(PI/16.f * j);
			step = dir * n * (j + 1);
			ivec2 texSize = textureSize(depthPyramid[i], 0);
			vec2 samplingCoord = (screenPos + step) / (n * texSize);
			vec3 texSample = texture(depthPyramid[i], samplingCoord).rgb;
//			vec3 texSample = customSampler(depthPyramid[i], samplingCoord).rgb;
			if (- pos.z - texSample.r < R * 0.5){
				z = texSample.g;
			} else {
				z = texSample.b;
			}
//			z = texSample.g;

			vec2 xyPos = xy + step*stepSize*2;
			currPos = toViewSpace(vec4(xyPos, 0.0f, 1.0f), z).rgb;
			currPos.z = -z;
			origToSample = currPos - pos;
			r = sqrt(dot(origToSample, origToSample));
			origToSample /= r;
			sinH = dot(normal, origToSample);
			if (r < R) {
//				float z_mul = min(1.0, 1.0 / (1 + pow(abs(currPos.z - pos.z), 0.10)));
//				z_mul = 1;
				ao = max(ao, sinH * attenuationFunc(r));
//				ao += sinH * attenuationFunc(r);
			}
		}
	}
//	ao = ao / (stepCount * subsamples);


	return ao;
}

void main() {
	vec3 normal = normalize((ubo.view * vec4(texture(normalsTex, inUV).xyz, 0.0)).xyz); //in view space
	float depth = texture(depthTex, inUV).r;
	float z = linearizeDepth(depth);
	vec4 pos = vec4(inUV*2.0 - 1.0, depth, 1.0f);
	pos = toViewSpace(pos, z);


	ivec2 screenCoord = toIntCoords(inUV, ivec2(invProjUBO.screenSize));
	vec2 noiseCoord = toFloatCoords(screenCoord % 32, vec2(1.0f/32.0f));
	float rotAngle = texture(blueNoiseTex, noiseCoord).r * 2*PI;
	mat2 dirRotMat = rotMat(rotAngle);

	vec2 dirs[DIR_COUNT];
	for (int i = 0; i < DIR_COUNT; ++i) {
		dirs[i] = dirRotMat * const_dirs[i];
	}


	float ao = 0.0f;
	vec2 pixelSize = 1.0f / invProjUBO.screenSize;
	for (int i = 0; i < DIR_COUNT; ++i) {
		ao += pyramidAoInDirection(dirs[i], pos.xyz+0.001*normal, normal, pixelSize);
	}
	ao = 1 - ao/DIR_COUNT;



//	 outAO = vec4(normal, 1.0f);
//	outAO = texture(depthPyramid[0], inUV);
//	outAO = vec4(pos.xyz, 1.0f);
	 outAO = vec4(ao.rrr, 1.0f);
}