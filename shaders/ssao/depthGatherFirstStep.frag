#version 450

layout(location = 0) out vec4 outColor;

layout(location = 0) in  vec2 inUV;

layout(binding = 0) uniform sampler2D inputImg;

//TODO@: move this to uniform buffer
float zNear = 0.1f;
float zFar  = 100.0f;

float linearizeDepth(float depth) {
	float n = zNear;
	float f = zFar;

	return n*f/(f - depth*(f - n));
}

ivec2 toIntCoords(vec2 coords, ivec2 screenSize) {
	return ivec2(coords * screenSize - 0.45f);
}

vec2 toFloatCoords(ivec2 coords, vec2 pixelSize) {
	return (coords + 0.5f) * pixelSize;
}

void main() {
	ivec2 inputTexSize = textureSize(inputImg, 0);
	ivec2 outTexSize = (inputTexSize + 2) / 3;
	
	ivec2 outTexCoord = toIntCoords(inUV, outTexSize);
	ivec2 inputTexCoord = outTexCoord*3; //left up corner from 3x3 group
	vec2 inputTexPixelSize = 1.0f / inputTexSize;
	vec2 leftUpCoord = toFloatCoords(inputTexCoord, inputTexPixelSize);

	// gather info from 3x3 kernel
	float texSample = linearizeDepth(texture(inputImg, leftUpCoord).r);
	float _min  = texSample;
	float _max  = texSample;
	float _mean = texSample;

	vec2 currCoord;
	for (int j = 1; j < 3; ++j) {
		currCoord = leftUpCoord + vec2(inputTexPixelSize.x*j, 0);
		texSample = linearizeDepth(texture(inputImg, currCoord).r);
		_min = min(_min, texSample);
		_max = max(_max, texSample);
		_mean += texSample;
	}
	for (int i = 1; i < 3; ++i) {
		for (int j = 0; j < 3; ++j) {
			currCoord = leftUpCoord + inputTexPixelSize*vec2(j, i);
			texSample = linearizeDepth(texture(inputImg, currCoord).r);
			_min = min(_min, texSample);
			_max = max(_max, texSample);
			_mean += texSample;
		}
	}
	_mean /= 9.0f;

	outColor = vec4(_min, _mean, _max, 1.0f);
}