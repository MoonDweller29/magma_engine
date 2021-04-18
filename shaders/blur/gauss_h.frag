#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in  vec2 inUV;

layout(binding = 0) uniform sampler2D inputImg;

#define KERNEL_SIZE 7
float coeffs[KERNEL_SIZE] = {
	0.03663285, 0.11128076, 0.21674532,
	0.27068215, 0.21674532, 0.11128076, 0.03663285};

void main() {
	ivec2 texSize = textureSize(inputImg, 0);
	vec2 dir = vec2(1.0f / texSize.x, 0.0f);
	float res = 0.0f;
	int kernelHalf = 3;
	for (int i = 0; i < KERNEL_SIZE; ++i) {
		vec2 coord = (i - kernelHalf) * dir + inUV;
		res += texture(inputImg, coord).r * coeffs[i];
	}
	
	outColor = vec4(res.rrr, 1.0f);
}