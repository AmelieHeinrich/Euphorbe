#include "Mesh.h"

#include <cgltf.h>
#include <Euphorbe/Core/Log.h>

#define cgltf_call(call) do { cgltf_result _result = (call); assert(_result == cgltf_result_success); } while(0)

u32 CGLTFComponentTypeSize(cgltf_component_type type)
{
    switch (type)
    {
    case cgltf_component_type_r_8:
    case cgltf_component_type_r_8u:
        return 1;
    case cgltf_component_type_r_16:
    case cgltf_component_type_r_16u:
        return 2;
    case cgltf_component_type_r_32u:
    case cgltf_component_type_r_32f:
        return 4;
    }

    assert(0);
    return 0;
}

u32 CLTFComponentTypeCount(cgltf_type type)
{
    switch (type)
    {
    case cgltf_type_scalar:
        return 1;
    case cgltf_type_vec2:
        return 2;
    case cgltf_type_vec3:
        return 3;
    case cgltf_type_vec4:
    case cgltf_type_mat2:
        return 4;
    case cgltf_type_mat3:
        return 9;
    case cgltf_type_mat4:
        return 16;
    }

    assert(0);
    return 0;
}

u32 CountGLTFNodePrimitives(cgltf_node* node)
{
    u32 count = 0;

    if (node->mesh)
    {
        count += (u32)node->mesh->primitives_count;
    }

    for (i32 i = 0; i < node->children_count; i++)
    {
        count += CountGLTFNodePrimitives(node->children[i]);
    }

    return count;
}

void* QueryAccessorData(cgltf_accessor* accessor, u32* component_size, u32* component_count)
{
    *component_size = CGLTFComponentTypeSize(accessor->component_type);
    *component_count = CLTFComponentTypeCount(accessor->type);

    cgltf_buffer_view* view = accessor->buffer_view;
    return OFFSET_PTR_BYTES(void, view->buffer->data, view->offset);
}


void ProcessGLTFPrimitive(cgltf_primitive* primitive, u32 currentPrimitiveIndex, E_Submesh* mesh, E_Mesh* main_mesh)
{
    cgltf_attribute* position_attribute = 0;
    cgltf_attribute* texcoord_attribute = 0;
    cgltf_attribute* normal_attribute = 0;

    for (i32 attribute_index = 0; attribute_index < primitive->attributes_count; attribute_index++)
    {
        cgltf_attribute* attribute = &primitive->attributes[attribute_index];

        if (strcmp(attribute->name, "POSITION") == 0) position_attribute = attribute;
        if (strcmp(attribute->name, "TEXCOORD_0") == 0) texcoord_attribute = attribute;
        if (strcmp(attribute->name, "NORMAL") == 0) normal_attribute = attribute;
    }

    assert(position_attribute && texcoord_attribute && normal_attribute);

    u32 vertex_stride_count = 8;
    u32 vertex_stride_bytes = vertex_stride_count * sizeof(f32);
    u32 vertex_count = (u32)position_attribute->data->count;
    u32 vertices_size = vertex_count * vertex_stride_bytes;
    f32* vertices = (f32*)malloc(vertices_size);

    {
        u32 component_size, component_count;
        f32* src = (f32*)QueryAccessorData(position_attribute->data, &component_size, &component_count);
        assert(component_size == 4);

        for (i32 vertex_index = 0; vertex_index < vertex_count; vertex_index++)
        {
            u32 internal_index = vertex_index * vertex_stride_count;
            vertices[internal_index + 0] = src[vertex_index * component_count + 0];
            vertices[internal_index + 1] = src[vertex_index * component_count + 1];
            vertices[internal_index + 2] = src[vertex_index * component_count + 2];
        }
    }

    {
        u32 component_size, component_count;
        f32* src = (f32*)QueryAccessorData(normal_attribute->data, &component_size, &component_count);
        assert(component_size == 4);

        for (i32 vertex_index = 0; vertex_index < vertex_count; vertex_index++)
        {
            u32 internal_index = vertex_index * vertex_stride_count;
            vertices[internal_index + 3] = src[vertex_index * component_count + 0];
            vertices[internal_index + 4] = src[vertex_index * component_count + 1];
            vertices[internal_index + 5] = src[vertex_index * component_count + 2];
        }
    }

    {
        u32 component_size, component_count;
        f32* src = (f32*)QueryAccessorData(texcoord_attribute->data, &component_size, &component_count);
        assert(component_size == 4);

        for (i32 vertex_index = 0; vertex_index < vertex_count; vertex_index++)
        {
            u32 internal_index = vertex_index * vertex_stride_count;
            vertices[internal_index + 6] = src[vertex_index * component_count + 0];
            vertices[internal_index + 7] = src[vertex_index * component_count + 1];
        }
    }

    u32 index_count = (u32)primitive->indices->count;
    u32* indices = (u32*)malloc(index_count * sizeof(u32));

    {
        u32 component_size, component_count;
        void* src = QueryAccessorData(primitive->indices, &component_size, &component_count);
        assert(component_size == 4 || component_size == 2);

        if (component_size == 2)
        { // u16
            u16* ptr = (u16*)src;
            for (i32 index_index = 0; index_index < index_count; index_index++)
            {
                indices[index_index] = ptr[index_index];
            }
        }
        else
        { // u32
            u32* ptr = (u32*)src;
            for (i32 index_index = 0; index_index < index_count; index_index++)
            {
                indices[index_index] = ptr[index_index];
            }
        }
    }

    mesh->vertex_buffer = E_CreateVertexBuffer(vertices_size);
    E_SetBufferData(mesh->vertex_buffer, vertices, vertices_size);

    mesh->index_buffer = E_CreateIndexBuffer(index_count * sizeof(u32));
    E_SetBufferData(mesh->index_buffer, indices, index_count * sizeof(u32));

    mesh->vertex_count = vertices_size / sizeof(vertices[0]);
    mesh->index_count = index_count;
    mesh->tri_count = mesh->vertex_count / 3;
    mesh->vertices_size = vertices_size;
    mesh->indices_size = index_count * sizeof(u32);

    main_mesh->total_vertex_count += mesh->vertex_count;
    main_mesh->total_index_count += mesh->index_count;
    main_mesh->total_tri_count += mesh->tri_count;

    free(vertices);
    free(indices);

    currentPrimitiveIndex++;
}

void ProcessGLTFNode(cgltf_node* node, u32 currentPrimitiveIndex, E_Mesh* mesh)
{
    if (node->mesh)
    {
        for (i32 primitiveIndex = 0; primitiveIndex < node->mesh->primitives_count; primitiveIndex++)
        {
            ProcessGLTFPrimitive(&node->mesh->primitives[primitiveIndex], currentPrimitiveIndex, &mesh->submeshes[primitiveIndex], mesh);
            mesh->submesh_count++;
        }
    }

    for (i32 childIndex = 0; childIndex < node->children_count; childIndex++)
    {
        ProcessGLTFNode(node->children[childIndex], currentPrimitiveIndex, mesh);
    }
}

E_Mesh* E_LoadMesh(const char* path)
{
    E_Mesh* mesh = malloc(sizeof(E_Mesh));
    mesh->submesh_count = 0;
    mesh->total_tri_count = 0;
    mesh->total_index_count = 0;
    mesh->total_vertex_count = 0;
    memset(mesh->submeshes, 0, sizeof(mesh->submeshes));
    
    cgltf_options options;
    memset(&options, 0, sizeof(cgltf_options));
    cgltf_data* data = 0;

    cgltf_call(cgltf_parse_file(&options, path, &data));
    cgltf_call(cgltf_load_buffers(&options, data, path));
    cgltf_scene* scene = data->scene;

    u32 primitive_count = 0;

    for (i32 node_index = 0; node_index < scene->nodes_count; node_index++)
    {
        primitive_count += CountGLTFNodePrimitives(scene->nodes[node_index]);
    }

    u32 primitive_index = 0;

    for (i32 node_index = 0; node_index < scene->nodes_count; node_index++)
    {
        ProcessGLTFNode(scene->nodes[node_index], primitive_index, mesh);
    }

    cgltf_free(data);

    return mesh;
}

void E_FreeMesh(E_Mesh* mesh)
{
    for (i32 i = 0; i < mesh->submesh_count; i++)
    {
        E_FreeBuffer(mesh->submeshes[i].vertex_buffer);
        E_FreeBuffer(mesh->submeshes[i].index_buffer);
    }

    free(mesh);
}
