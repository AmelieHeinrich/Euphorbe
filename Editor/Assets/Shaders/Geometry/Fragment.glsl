#version 450

#define MAX_LIGHT_COUNT 64
#define PI 3.14159265359

layout (location = 0) out vec4 OutColor;

layout (location = 0) in PerVertexData {
	vec2 OutUV;
	vec3 OutNormals;
	vec3 WorldPos;
	vec3 CameraPos;
} FragmentIn;

struct PointLight
{
    vec4 position;
    vec4 color;
};

// Uniforms
layout (binding = 1, set = 0) uniform MaterialSettings {
    bool has_albedo;
    bool has_metallic_roughness;
    bool has_normal;
    bool has_ao;
} settings;

layout (binding = 2, set = 0) uniform sampler TextureSampler;
layout (binding = 3, set = 0) uniform texture2D AlbedoTexture;
layout (binding = 4, set = 0) uniform texture2D MetallicRoughnessTexture;
layout (binding = 5, set = 0) uniform texture2D NormalTexture;
layout (binding = 6, set = 0) uniform texture2D AOTexture;

layout (binding = 0, set = 1) uniform Lights {
    PointLight lights[MAX_LIGHT_COUNT];
} light_settings;

layout (binding = 1, set = 1) uniform sampler cubemap_sampler;
layout (binding = 2, set = 1) uniform sampler brdf_sampler;
layout (binding = 3, set = 1) uniform textureCube Skybox;
layout (binding = 4, set = 1) uniform textureCube irradianceMap;
layout (binding = 5, set = 1) uniform textureCube prefilterMap;
layout (binding = 6, set = 1) uniform texture2D brdfLut;

vec3 GetNormalFromMap()
{
    vec3 tangentNormal = texture(sampler2D(NormalTexture, TextureSampler), FragmentIn.OutUV).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(FragmentIn.WorldPos);
    vec3 Q2  = dFdy(FragmentIn.WorldPos);
    vec2 st1 = dFdx(FragmentIn.OutUV);
    vec2 st2 = dFdy(FragmentIn.OutUV);

    vec3 N   = normalize(FragmentIn.OutNormals);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / max(denom, 0.0000001); // prevent divide by zero for roughness=0.0 and NdotH=1.0
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

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

vec3 FresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}   

void main()
{
    vec3 albedo;

    if (settings.has_albedo)
        albedo = pow(texture(sampler2D(AlbedoTexture, TextureSampler), FragmentIn.OutUV).rgb, vec3(2.2));
    else
        albedo = vec3(0.0f);

    float metallic = 1.0f;
    float roughness = 1.0f;
    float ao = 1.0f;
    vec3 N = normalize(FragmentIn.OutNormals);

    if (settings.has_metallic_roughness)
    {
        metallic = texture(sampler2D(MetallicRoughnessTexture, TextureSampler), FragmentIn.OutUV).b;
        roughness = texture(sampler2D(MetallicRoughnessTexture, TextureSampler), FragmentIn.OutUV).g;
    }
    if (settings.has_normal)
        N = GetNormalFromMap();

    // Calculate color
    vec3 V = normalize(FragmentIn.CameraPos - FragmentIn.WorldPos);
    vec3 R = reflect(-V, N); 

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 Lo = vec3(0.0);
    for(int i = 0; i < MAX_LIGHT_COUNT; i++) 
    {
        // calculate per-light radiance
        vec3 L = normalize(light_settings.lights[i].position.xyz - FragmentIn.WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(L);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light_settings.lights[i].color.xyz * vec3(attenuation);

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);    
        vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);        
        
        vec3 numerator    = NDF * G * F;
        float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	                
            
        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }   

    vec3 F = FresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;

    vec3 irradiance = texture(samplerCube(irradianceMap, cubemap_sampler), N).rgb;
    vec3 diffuse = irradiance * albedo;

    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(samplerCube(prefilterMap, cubemap_sampler), R, roughness * MAX_REFLECTION_LOD).rgb;   
    vec2 brdf_uv = vec2(max(dot(N, V), 0.0), roughness);
    vec2 brdf  = texture(sampler2D(brdfLut, brdf_sampler), brdf_uv).rg;
    vec3 specular = prefilteredColor * (F * brdf.x + brdf.y);

    vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 color = ambient + Lo;

    OutColor = vec4(color, 1.0);
}