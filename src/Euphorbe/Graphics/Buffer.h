#pragma once

#include <Euphorbe/Core/Common.h>

typedef enum E_BufferUsage E_BufferUsage;
enum E_BufferUsage
{
    E_BufferUsageVertex,
    E_BufferUsageIndex,
    E_BufferUsageUniform,
    E_BufferUsageStorage
};

typedef struct E_Buffer E_Buffer;
struct E_Buffer
{
    E_BufferUsage usage;
    void* rhi_handle; // E_VulkanBuffer for example  
};

E_Buffer* E_CreateVertexBuffer(i64 size);
E_Buffer* E_CreateIndexBuffer(i64 size);
E_Buffer* E_CreateUniformBuffer(i64 size);
E_Buffer* E_CreateStorageBuffer(i64 size);

void E_SetBufferData(E_Buffer* buffer, void* data, i64 size);
void E_FreeBuffer(E_Buffer* buffer);