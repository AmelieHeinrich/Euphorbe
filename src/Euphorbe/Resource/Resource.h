#ifndef EUPHORBE_RESOURCE_H
#define EUPHORBE_RESOURCE_H

#include <Euphorbe/Graphics/ShaderCompiler.h>

typedef enum E_ResourceType E_ResourceType;
enum E_ResourceType
{
    E_ResourceTypeUndefined = 0,
    E_ResourceTypeVertexShader,
    E_ResourceTypeFragmentShader
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
    } as;
};

E_ResourceFile* E_LoadResource(const char* path, E_ResourceType type);
void E_FreeResource(E_ResourceFile* file);

#endif