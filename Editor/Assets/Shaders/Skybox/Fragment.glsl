#version 450

layout (location = 0) out vec4 OutColor;

layout (location = 0) in vec2 OutUV;
layout (location = 1) in vec3 OutPosition;

layout (binding = 0) uniform samplerCube CubemapTexture;

void main()
{
    OutColor = vec4(texture(CubemapTexture, OutPosition).rgb, 1.0);
}