#version 450

layout(location = 0) out float outColor;

layout(location = 0) in  vec2 inUV;

layout(binding = 0) uniform sampler2D inputImg;

#define KERNEL_SIZE 7
float coeffs[KERNEL_SIZE] = {
	0.03663285, 0.11128076, 0.21674532,
	0.27068215, 0.21674532, 0.11128076, 0.03663285};

void main() {
	ivec2 texSize = textureSize(inputImg, 0);
	vec2 dir = vec2(0.0f, 1.0f / texSize.y);
	float res = 0.0f;
	int kernelHalf = 3;
	for (int i = 0; i < KERNEL_SIZE; ++i) {
		vec2 coord = (i - kernelHalf) * dir + inUV;
		res += texture(inputImg, coord).r * coeffs[i];
	}
	outColor = res;
}