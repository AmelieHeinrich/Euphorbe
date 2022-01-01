#version 450

#define MAX_LIGHT_COUNT 64
#define PI 3.14159265359

layout (location = 0) out vec4 OutColor;

layout (location = 0) in vec3 OutPos;
layout (location = 1) in vec2 OutUV;
layout (location = 2) in vec3 OutNormals;
layout (location = 3) in vec4 WorldPos;
layout (location = 4) in vec3 CameraPos;

struct PointLight
{
    vec4 position;
    vec4 color;
};

// Uniforms
layout (binding = 1, set = 0) uniform MaterialSettings {
    bool has_albedo;
    bool has_metallic_roughness;
    bool enable_blending;
    bool enable_reflection;
} settings;

layout (binding = 2, set = 0) uniform sampler2D AlbedoTexture;
layout (binding = 3, set = 0) uniform sampler2D MetallicRoughnessTexture;

layout (binding = 0, set = 1) uniform Lights {
    PointLight lights[MAX_LIGHT_COUNT];
} light_settings;

layout (binding = 1, set = 1) uniform samplerCube Skybox;
layout (binding = 2, set = 1) uniform samplerCube irradianceMap;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 FresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{
    vec4 albedo_color;

    if (settings.has_albedo)
        albedo_color = texture(AlbedoTexture, OutUV);
    else
        albedo_color = vec4(1.0f);

    if (settings.enable_blending)
    {
        if (albedo_color.a < 0.25)
            discard;
    }

    float metallic = 1.0f;
    float roughness = 1.0f;

    if (settings.has_metallic_roughness)
    {
        metallic = texture(MetallicRoughnessTexture, OutUV).g;
        roughness = texture(MetallicRoughnessTexture, OutUV).r;
    }

    // Calculate color
    vec3 N = normalize(OutNormals);
    vec3 V = normalize(CameraPos - WorldPos.xyz);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo_color.rgb, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < MAX_LIGHT_COUNT; i++) 
    {
        vec3 L = normalize(light_settings.lights[i].position.xyz - WorldPos.xyz);
        vec3 H = normalize(V + L);
        float distance = length(light_settings.lights[i].position.xyz - WorldPos.xyz);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light_settings.lights[i].color.rgb * attenuation;

        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        vec3 specular = numerator / denominator;

        vec3 kS = F;
        vec3 kD = vec3(1.0) - kS;
        kD *= 1.0 - metallic;	  

        float NdotL = max(dot(N, L), 0.0);        

        Lo += (kD * albedo_color.xyz / PI + specular) * radiance * NdotL; 
    }   

    vec3 kS = FresnelSchlick(max(dot(N, V), 0.0), F0);
    vec3 kD = 1.0 - kS;
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo_color.xyz;
    vec3 ambient = (kD * diffuse);
    vec3 color = ambient + Lo;

    OutColor = vec4(color, 1.0);
}