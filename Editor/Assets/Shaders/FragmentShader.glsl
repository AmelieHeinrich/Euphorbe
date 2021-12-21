#version 450

layout (location = 0) out vec4 OutColor;

layout(binding = 0) uniform ColorUniform {
    vec3 color;
} color;

void main()
{
    OutColor = vec4(color.color, 1.0);
}