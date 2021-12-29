#version 450

layout (location = 0) out vec4 OutColor;

layout (location = 0) in vec2 OutUV;
layout (location = 1) in vec3 OutPosition;

layout(binding = 0) uniform sampler2D HDRTexture;

const vec2 inv_atan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= inv_atan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(OutPosition));

    // Apply gamma correction
    const float gamma = 2.2;
    vec3 hdr_color = texture(HDRTexture, uv).rgb;

    float map_x = hdr_color.x / (hdr_color.x + 1.0);
	float map_y = hdr_color.y / (hdr_color.y + 1.0);
	float map_z = hdr_color.z / (hdr_color.z + 1.0);
	vec3 map = vec3(map_x, map_y, map_z);

    map.x = pow(abs(map.x), 1.0 / gamma);
	map.y = pow(abs(map.y), 1.0 / gamma);
	map.z = pow(abs(map.z), 1.0 / gamma);

    OutColor = vec4(map, 1.0);
}