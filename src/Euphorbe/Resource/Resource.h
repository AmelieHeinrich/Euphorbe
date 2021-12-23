#ifndef EUPHORBE_RESOURCE_H
#define EUPHORBE_RESOURCE_H

#include <Euphorbe/Graphics/ShaderCompiler.h>
#include <Euphorbe/Graphics/Image.h>

typedef struct E_Material E_Material;

typedef enum E_ResourceType E_ResourceType;
enum E_ResourceType
{
    E_ResourceTypeUndefined = 0,
    E_ResourceTypeVertexShader,
    E_ResourceTypeFragmentShader,
    E_ResourceTypeTexture,
    E_ResourceTypeMaterial
};

typedef struct E_ResourceFile E_ResourceFile;
struct E_ResourceFile
{
    E_ResourceType type;
    char* path;

    char* resource_data;
    i32 resource_size;

    struct {
        E_Shader* shader;
        E_Image* image;
        E_Material* material;
    } as;
};

E_ResourceFile* E_LoadResource(const char* path, E_ResourceType type);
void E_FreeResource(E_ResourceFile* file);

#endif