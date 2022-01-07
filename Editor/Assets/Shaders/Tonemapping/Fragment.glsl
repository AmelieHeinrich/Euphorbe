#version 450

#extension GL_KHR_vulkan_glsl: enable

layout (location = 0) out vec4 OutColor;

layout (location = 0) in vec3 OutPosition;
layout (location = 1) in vec2 OutUV;

layout (push_constant) uniform HDRSettings {
	bool gamma_correction;
	int mode;

	vec2 padding_0;
} settings;

layout (binding = 0, set = 0) uniform sampler screen_sampler;
layout (binding = 1, set = 0) uniform texture2D color_texture;

// Tonemapping curves: https://www.shadertoy.com/view/lslGzl
// ACES: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
// ACES impl: https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl

vec3 aces(vec3 color, float gamma) 
{
	const mat3 inputMatrix = mat3
    (
		vec3(0.59719, 0.07600, 0.02840),
		vec3(0.35458, 0.90834, 0.13383),
		vec3(0.04823, 0.01566, 0.83777)
    );
    
    const mat3 outputMatrix = mat3
    (
		vec3(1.60475, -0.10208, -0.00327),
		vec3(-0.53108, 1.10813, -0.07276),
		vec3(-0.07367, -0.00605, 1.07602)
    );
    
    vec3 inputColour = inputMatrix * color;
    vec3 a = inputColour * (inputColour + vec3(0.0245786)) - vec3(0.000090537);
    vec3 b = inputColour * (0.983729 * inputColour + 0.4329510) + 0.238081;
    vec3 c = a / b;
    return pow(max(outputMatrix * c, 0.0.xxx), vec3(1. / gamma));
}

vec3 filmic(vec3 color, float gamma) 
{
	color = max(vec3(0.), color - vec3(0.004));
	color = (color * (6.2 * color + .5)) / (color * (6.2 * color + 1.7) + 0.06);
	return pow(color, vec3(1. / gamma));
}

vec3 reinhard(vec3 color, float gamma)
{
	float exposure = 1.5;
	color *= exposure/(1. + color / exposure);
	color = pow(color, vec3(1.0 / gamma));
	return color;
}

vec3 lumaReinhard(vec3 color, float gamma) 
{
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 whitePreservingLumaReinhard(vec3 color, float gamma)
{
	float white = 2.;
	float luma = dot(color, vec3(0.2126, 0.7152, 0.0722));
	float toneMappedLuma = luma * (1. + luma / (white*white)) / (1. + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 romBinDaHouse(vec3 color, float gamma)
{
	color = exp( -1.0 / ( 2.72*color + 0.15 ) );
	color = pow(color, vec3(1. / gamma));
	return color;
}

vec3 uncharted2(vec3 color, float gamma) {
	float A = 0.15;
	float B = 0.50;
	float C = 0.10;
	float D = 0.20;
	float E = 0.02;
	float F = 0.30;
	float W = 11.2;
	float exposure = 2.0;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, vec3(1.0 / gamma));
	return color;
}

void main()
{
	vec3 color = texture(sampler2D(color_texture, screen_sampler), OutUV).rgb;
	float gamma;

	if (settings.gamma_correction)
		gamma = 2.2;
	else
		gamma = 1.0;
	
	switch (settings.mode)
	{
	case 0: // aces
		color.rgb = aces(color.rgb, gamma);
		break;
	case 1: // reinhard
		color.rgb = reinhard(color.rgb, gamma);
		break;
	case 2: // luma reinhard
		color.rgb = lumaReinhard(color.rgb, gamma);
		break;
	case 3: // white perserving luma reinhard
		color.rgb = whitePreservingLumaReinhard(color.rgb, gamma);
		break;
	case 4: // rom bin da house
		color.rgb = romBinDaHouse(color.rgb, gamma);
		break;
	case 5: // filmic
		color.rgb = filmic(color.rgb, gamma);
		break;
	case 6: // uncharted2
		color.rgb = uncharted2(color.rgb, gamma);
		break;
	case 7: // none
		color.rgb = pow(color.rgb, vec3(1. / gamma));
		break;
	}

	OutColor = vec4(color, 1.0);
}