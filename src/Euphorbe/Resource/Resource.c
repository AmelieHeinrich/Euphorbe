#include "Resource.h"

#include <stdio.h>
#include <stdlib.h>

#include <Euphorbe/Core/Log.h>
#include <Euphorbe/Graphics/Material.h>

E_ResourceFile* E_LoadResource(const char* path, E_ResourceType type)
{
    E_ResourceFile* resource = malloc(sizeof(E_ResourceFile));
	resource->type = type;
	resource->path = (char*)path;
	if (type != E_ResourceTypeMesh)
		resource->resource_data = E_ReadFile(path, &resource->resource_size);

	switch (resource->type)
	{
	case E_ResourceTypeUndefined:
		break;
	case E_ResourceTypeVertexShader:
		resource->as.shader = malloc(sizeof(E_Shader));
		if (resource->as.shader)
		{
			resource->as.shader->type = E_ShaderTypeVertex;
			E_CompileShader(resource->resource_data, resource->resource_size, resource->as.shader);
			break;
		}
		else
		{
			E_LogError("RESOURCE SHADER ALLOCATION: Failed to allocate resource shader!");
			assert(0);
			return NULL;
		}
		break;
	case E_ResourceTypeFragmentShader:
		resource->as.shader = malloc(sizeof(E_Shader));
		if (resource->as.shader)
		{
			resource->as.shader->type = E_ShaderTypeFragment;
			E_CompileShader(resource->resource_data, resource->resource_size, resource->as.shader);
			break;
		}
		else
		{
			E_LogError("RESOURCE SHADER ALLOCATION: Failed to allocate resource shader!");
			assert(0);
			return NULL;
		}
		break;
	case E_ResourceTypeTexture:
		resource->as.image = E_MakeImageFromFile(resource->path);
		break;
	case E_ResourceTypeMaterial:
		resource->as.material = E_CreateMaterialFromFile(resource->path);
		break;
	case E_ResourceTypeMesh:
		resource->as.mesh = E_LoadMesh(resource->path);
		break;
	}

	return resource;
}

void E_FreeResource(E_ResourceFile* file)
{
	switch (file->type)
	{
	case E_ResourceTypeVertexShader:
		free(file->as.shader->code);
		free(file->as.shader);
		break;
	case E_ResourceTypeFragmentShader:
		free(file->as.shader->code);
		free(file->as.shader);
		break;
	case E_ResourceTypeTexture:
		E_FreeImage(file->as.image);
		break;
	case E_ResourceTypeMaterial:
		E_FreeMaterial(file->as.material);
		break;
	case E_ResourceTypeMesh:
		E_FreeMesh(file->as.mesh);
		break;
	case E_ResourceTypeUndefined:
		break;
	}

	if (file->type != E_ResourceTypeMesh)
		free(file->resource_data);
    free(file);
}
