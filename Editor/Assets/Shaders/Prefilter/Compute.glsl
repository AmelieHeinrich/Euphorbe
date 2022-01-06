#version 450

// CREDIT: IronFist, Strontium Engine
// https://github.com/tuxalin/vulkanri/blob/master/examples/pbr_ibl/shaders/prefilterGGX.comp

/*
 * A compute shader to convert an equirectangular map to a cubemap. Many thanks
 * to http://alinloghin.com/articles/compute_ibl.html, some of the code here was
 * adapted from their excellent article.
*/

layout (local_size_x = 32, local_size_y = 32) in;

layout (binding = 0) uniform samplerCube environmentMap;
layout (rgba16f, binding = 1) writeonly uniform imageCube prefilterMap;

layout (push_constant) uniform ParameterBuffer {
	vec4 u_iblParams;
};

#define PI 3.141592654

vec3 cubeToWorld(ivec3 cubeCoord, vec2 cubeSize);
ivec3 texToCube(vec3 texCoord, vec2 cubeSize);
float SSBGeometry(vec3 N, vec3 H, float roughness);
vec2 Hammersley(uint i, uint N);
vec3 SSBImportance(vec2 Xi, vec3 N, float roughness);

void main()
{
    const uint SAMPLE_COUNT = 1;
    vec2 enviSize = vec2(textureSize(environmentMap, 0).xy);
    vec2 mipSize = vec2(imageSize(prefilterMap).xy);
    ivec3 cubeCoord = ivec3(gl_GlobalInvocationID);
    
    vec3 worldPos = cubeToWorld(cubeCoord, mipSize);
    vec3 N = normalize(worldPos);
    
    vec3 prefilteredColor = vec3(0.0);
    float totalWeight = 0.0;
    
    for (uint i = 0u; i < SAMPLE_COUNT; ++i)
    {
          vec2 Xi = Hammersley(i, SAMPLE_COUNT);
          vec3 H = SSBImportance(Xi, N, u_iblParams.x);
          vec3 L  = normalize(2.0 * dot(N, H) * H - N);
    
          float NdotL = max(dot(N, L), 0.0);
          if (NdotL > 0.0)
          {
              float D = SSBGeometry(N, H, u_iblParams.x);
              float NdotH = max(dot(N, H), 0.0);
              float HdotV = max(dot(H, N), 0.0);
              float pdf = D * NdotH / (4.0 * HdotV + 0.0001);
    
              float saTexel  = 4.0 * PI / (6.0 * enviSize.x * enviSize.x);
              float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);
    
              float mipLevel = u_iblParams.x == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);
    
              prefilteredColor += textureLod(environmentMap, L, mipLevel).rgb * NdotL;
              totalWeight += NdotL;
          }
    }
    
    prefilteredColor = prefilteredColor / totalWeight;
    
    imageStore(prefilterMap, cubeCoord, vec4(prefilteredColor, 1.0));
}

// I need to figure out how to make these branchless one of these days...
vec3 cubeToWorld(ivec3 cubeCoord, vec2 cubeSize)
{
    vec2 texCoord = vec2(cubeCoord.xy) / cubeSize;
    texCoord = texCoord  * 2.0 - 1.0; // Swap to -1 -> +1
    switch(cubeCoord.z)
    {
        case 0: return vec3(1.0, -texCoord.yx); // CUBE_MAP_POS_X
        case 1: return vec3(-1.0, -texCoord.y, texCoord.x); // CUBE_MAP_NEG_X
        case 2: return vec3(texCoord.x, 1.0, texCoord.y); // CUBE_MAP_POS_Y
        case 3: return vec3(texCoord.x, -1.0, -texCoord.y); // CUBE_MAP_NEG_Y
        case 4: return vec3(texCoord.x, -texCoord.y, 1.0); // CUBE_MAP_POS_Z
        case 5: return vec3(-texCoord.xy, -1.0); // CUBE_MAP_NEG_Z
    }
    return vec3(0.0);
}

ivec3 texToCube(vec3 texCoord, vec2 cubeSize)
{
    vec3 abst = abs(texCoord);
    texCoord /= max(max(abst.x, abst.y), abst.z);
    
    float cubeFace;
    vec2 uvCoord;
    if (abst.x > abst.y && abst.x > abst.z)
    {
        // X major.
        float negx = step(texCoord.x, 0.0);
        uvCoord = mix(-texCoord.zy, vec2(texCoord.z, -texCoord.y), negx);
        cubeFace = negx;
    }
    else if (abst.y > abst.z)
    {
        // Y major.
        float negy = step(texCoord.y, 0.0);
        uvCoord = mix(texCoord.xz, vec2(texCoord.x, -texCoord.z), negy);
        cubeFace = 2.0 + negy;
    }
    else
    {
        // Z major.
        float negz = step(texCoord.z, 0.0);
        uvCoord = mix(vec2(texCoord.x, -texCoord.y), -texCoord.xy, negz);
        cubeFace = 4.0 + negz;
    }
    uvCoord = (uvCoord + 1.0) * 0.5; // 0..1
    uvCoord = uvCoord * cubeSize;
    uvCoord = clamp(uvCoord, vec2(0.0), cubeSize - vec2(1.0));
    
    return ivec3(ivec2(uvCoord), int(cubeFace));
}

// Smith-Schlick-Beckmann geometry function.
float SSBGeometry(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float a2     = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom       = PI * denom * denom;
    
    return nom / denom;
}

// Hammersley sequence, generates a low discrepancy pseudorandom number.
vec2 Hammersley(uint i, uint N)
{
    float fbits;
    uint bits = i;
    
    bits  = (bits << 16u) | (bits >> 16u);
    bits  = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
    bits  = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
    bits  = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
    bits  = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
    fbits = float(bits) * 2.3283064365386963e-10;
    
    return vec2(float(i) / float(N), fbits);
}

// Importance sampling of the Smith-Schlick-Beckmann geometry function.
vec3 SSBImportance(vec2 Xi, vec3 N, float roughness)
{
	float a = roughness * roughness;

	float phi = 2.0 * PI * Xi.x;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a*a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	// from spherical coordinates to cartesian coordinates - halfway vector
	vec3 H;
	H.x = cos(phi) * sinTheta;
	H.y = sin(phi) * sinTheta;
	H.z = cosTheta;

	// from tangent-space H vector to world-space sample vector
	vec3 up        = abs(N.z) < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 tangent   = normalize(cross(up, N));
	vec3 bitangent = cross(N, tangent);

	vec3 sampleVec = tangent * H.x + bitangent * H.y + N * H.z;
	return normalize(sampleVec);
}