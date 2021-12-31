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

    vec3 color = texture(HDRTexture, uv).rgb;
    OutColor = vec4(color, 1.0);
}