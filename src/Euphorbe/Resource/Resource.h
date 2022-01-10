#pragma once

#include <Euphorbe/Graphics/ShaderCompiler.h>
#include <Euphorbe/Graphics/Image.h>

typedef struct E_CommandBuffer E_CommandBuffer;

typedef struct E_Material E_Material;

typedef enum E_ResourceType E_ResourceType;
enum E_ResourceType
{
    E_ResourceTypeUndefined = 0,
    E_ResourceTypeVertexShader,
    E_ResourceTypeGeometryShader,
    E_ResourceTypeFragmentShader,
    E_ResourceTypeComputeShader,
    E_ResourceTypeTaskShader,
    E_ResourceTypeMeshShader,
    E_ResourceTypeTexture,
    E_ResourceTypeMaterial,
    E_ResourceTypeComputeMaterial
};

typedef struct E_ResourceFile E_ResourceFile;
struct E_ResourceFile
{
    E_ResourceType type;
    char* path;
    i32 resource_size;

    struct {
        E_Shader* shader;
        E_Image* image;
        E_Material* material;
    } as;
};

E_ResourceFile* E_LoadResource(const char* path, E_ResourceType type);
void E_FreeResource(E_ResourceFile* file);