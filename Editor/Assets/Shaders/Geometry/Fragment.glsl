#version 450

layout (location = 0) out vec4 OutColor;
layout (location = 0) in vec2 OutUV;

layout (binding = 0) uniform MaterialSettings {
    bool has_albedo;
    bool has_metallic_roughness;
    bool enable_blending;
    bool flip_uvs;
} settings;

layout (binding = 1) uniform sampler2D OurTexture;

void main()
{
    vec2 uvs;
    if (settings.flip_uvs)
        uvs = vec2(OutUV.x, -OutUV.y);
    else
        uvs = OutUV;

    vec4 albedo_color;

    if (settings.has_albedo)
        albedo_color = texture(OurTexture, OutUV);
    else
        albedo_color = vec4(1.0f);

    if (settings.enable_blending)
    {
        if (albedo_color.a < 0.25)
            discard;
    }

    OutColor = albedo_color;
}