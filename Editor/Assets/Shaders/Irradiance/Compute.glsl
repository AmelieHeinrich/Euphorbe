#version 450

// CREDIT: IronFist, Strontium Engine
// https://github.com/ksawatzky777/Strontium/blob/main/assets/shaders/compute/ibl/diffuseConv.srshader

/*
 * A compute shader to convert an equirectangular map to a cubemap. Many thanks
 * to http://alinloghin.com/articles/compute_ibl.html, some of the code here was
 * adapted from their excellent article.
*/

layout (local_size_x = 32, local_size_y = 32) in;

layout (rgba32f, binding = 0) uniform readonly imageCube environmentMap;
layout (rgba32f, binding = 1) writeonly uniform imageCube irradianceMap;

#define PI 3.141592654

vec3 cubeToWorld(ivec3 cubeCoord, vec2 cubeSize);
ivec3 texToCube(vec3 texCoord, vec2 cubeSize);

void main()
{
	vec2 cubemapSize = vec2(imageSize(irradianceMap).xy);
    vec3 worldPos = cubeToWorld(ivec3(gl_GlobalInvocationID), cubemapSize);

    // The normal is the same as the worldspace position.
    vec3 normal = normalize(worldPos);

    // Other hemispherical directions required for the integral.
    vec3 up     = vec3(0.0, 1.0, 0.0);
    vec3 right  = cross(up, normal);
    up          = cross(normal, right);

    float sampleDelta = 0.025;
    float nrSamples = 0.0;
    vec3 irradiance = vec3(0.0);

    for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
    {
        for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
        {
            // Spherical to cartesian (in tangent space).
            vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
            // Tangent space to world.
            vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;
            ivec3 sampleCoord = texToCube(sampleVec, cubemapSize);

            irradiance += imageLoad(environmentMap, sampleCoord).rgb * cos(theta) * sin(theta);
            nrSamples++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(nrSamples));

    imageStore(irradianceMap, ivec3(gl_GlobalInvocationID), vec4(irradiance, 1.0));
}

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