#version 450

layout (location = 0) out vec4 OutColor;
layout (location = 0) in vec2 OutUV;

layout(binding = 1) uniform sampler2D OurTexture;

void main()
{
    vec4 color = texture(OurTexture, OutUV);

    if (color.a < 0.25)
        discard;

    OutColor = color;
}