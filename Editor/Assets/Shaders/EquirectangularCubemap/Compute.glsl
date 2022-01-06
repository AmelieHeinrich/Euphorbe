#version 450

// CREDIT: IronFist, Strontium Engine
// https://github.com/ksawatzky777/Strontium/blob/main/assets/shaders/compute/ibl/equiConversion.srshader

/*
 * A compute shader to convert an equirectangular map to a cubemap. Many thanks
 * to http://alinloghin.com/articles/compute_ibl.html, some of the code here was
 * adapted from their excellent article.
*/

#define invAtan vec2(0.1591, 0.3183)

layout (local_size_x = 32, local_size_y = 32) in;

layout (rgba16f, binding = 0) uniform readonly image2D equirectangularMap;
layout (rgba16f, binding = 1) writeonly uniform imageCube environmentMap;

vec3 cubeToWorld(ivec3 cubeCoord, vec2 cubeSize);
ivec3 texToCube(vec3 texCoord, vec2 cubeSize);
ivec2 texToImage2D(vec2 texCoord, vec2 imageSize);
vec2 sampleSphericalMap(vec3 v);

void main()
{
    // Fetch the world position of the cubemap.
    vec2 equiSize = vec2(imageSize(equirectangularMap));
    vec2 enviSize = vec2(imageSize(environmentMap).xy);
    vec3 worldPos = cubeToWorld(ivec3(gl_GlobalInvocationID), enviSize);
    
    // Convert the cubemap coords to equirectangular coords.
    vec2 uv = sampleSphericalMap(normalize(worldPos));
    // Convert the UV coordinates to image coordinates.
    ivec2 imgCoords = texToImage2D(uv, equiSize);
    
    // Fetch the colour and store it in the cubemap.
    vec3 colour = imageLoad(equirectangularMap, imgCoords).rgb;
    imageStore(environmentMap, ivec3(gl_GlobalInvocationID), vec4(colour, 1.0));
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

ivec2 texToImage2D(vec2 texCoord, vec2 imageSize)
{
    vec2 result = texCoord * imageSize;
    return ivec2(result);
}

vec2 sampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}