#version 450

layout (location = 0) in vec3 Color;

layout (location = 0) out vec4 OutColor;

void main()
{
    OutColor = vec4(Color, 1.0);
}