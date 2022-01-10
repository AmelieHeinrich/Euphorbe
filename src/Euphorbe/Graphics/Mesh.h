#pragma once

#define EUPHORBE_MAX_SUBMESHES 128
#define EUPHORBE_MAX_MESHLET_VERTICES 64
#define EUPHORBE_MAX_MESHLET_INDICES 372
#define EUPHORBE_MAX_MESHLET_TRIANGLES 124

#include <Euphorbe/Core/Common.h>
#include <cglm/cglm.h>

#include "Buffer.h"
#include "Material.h"

typedef struct E_MeshVertex E_MeshVertex;
struct E_MeshVertex
{
	vec3 Position;
	vec2 UV;
	vec3 Normals;
};

typedef struct E_Meshlet E_Meshlet;
struct E_Meshlet
{
	vec4 cone;
	u32 vertices[EUPHORBE_MAX_MESHLET_VERTICES];    
	u8 indices[EUPHORBE_MAX_MESHLET_INDICES];  
	u8 vertex_count; 
	u8 triangle_count;  
};

typedef struct E_Submesh E_Submesh;
struct E_Submesh
{
	E_Buffer* vertex_buffer;
	E_Buffer* index_buffer;
	E_Buffer* meshlet_buffer;

	E_MaterialInstance* geometry_instance;

	u32 vertices_size;
	u32 indices_size;

	u32 vertex_count;
	u32 index_count;
	u32 tri_count;
	u32 meshlet_count;
};

typedef struct E_Mesh E_Mesh;
struct E_Mesh
{
	E_Submesh submeshes[EUPHORBE_MAX_SUBMESHES];
	i32 submesh_count;

	u32 total_vertex_count;
	u32 total_index_count;
	u32 total_tri_count;
	u32 total_meshlet_count;
};

E_Mesh* E_LoadMesh(E_Material* material, const char* path);
void E_FreeMesh(E_Mesh* mesh);