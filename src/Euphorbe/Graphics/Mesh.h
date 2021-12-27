#ifndef EUPHORBE_MESH_H
#define EUPHORBE_MESH_H

#define EUPHORBE_MAX_SUBMESHES 128

#include <Euphorbe/Core/Common.h>
#include <cglm/cglm.h>

#include "Buffer.h"

typedef struct E_Submesh E_Submesh;
struct E_Submesh
{
	E_Buffer* vertex_buffer;
	E_Buffer* index_buffer;

	u32 vertices_size;
	u32 indices_size;

	u32 vertex_count;
	u32 index_count;
	u32 tri_count;
};

typedef struct E_Mesh E_Mesh;
struct E_Mesh
{
	E_Submesh submeshes[EUPHORBE_MAX_SUBMESHES];
	i32 submesh_count;

	u32 total_vertex_count;
	u32 total_index_count;
	u32 total_tri_count;
};

E_Mesh* E_LoadMesh(const char* path);
void E_FreeMesh(E_Mesh* mesh);

#endif