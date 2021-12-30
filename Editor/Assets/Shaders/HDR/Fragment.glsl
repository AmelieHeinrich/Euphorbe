#version 450

layout (location = 0) out vec4 OutColor;

layout (location = 0) in vec3 OutPosition;
layout (location = 1) in vec2 OutUV;

layout (push_constant) uniform HDRSettings {
	bool enabled;
	float exposure;

	vec2 pad_16;
} settings;

layout (binding = 0) uniform sampler2D color_texture;

void main()
{
	const float gamma = 2.2;
	vec3 hdr_color = texture(color_texture, OutUV).rgb;
	if (settings.enabled)
	{
		vec3 result = vec3(1.0) - exp(-hdr_color * settings.exposure);
		result = pow(result, vec3(1.0 / gamma));
		OutColor = vec4(result, 1.0);
	}
	else
	{
		vec3 result = pow(hdr_color, vec3(1.0 / gamma));
        OutColor = vec4(result, 1.0);
	}
}