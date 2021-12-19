#include "Buffer.h"

#ifdef EUPHORBE_WINDOWS
    #include <Euphorbe/Graphics/Vulkan/VulkanBuffer.h>
#endif

E_Buffer* E_CreateVertexBuffer(i64 size)
{
    E_Buffer* buffer = malloc(sizeof(E_Buffer));
    buffer->usage = E_BufferUsageVertex;

#ifdef EUPHORBE_WINDOWS
    buffer->rhi_handle = E_Vk_AllocateVertexBuffer(size);
#endif

    return buffer;
}

E_Buffer* E_CreateIndexBuffer(i64 size)
{
    E_Buffer* buffer = malloc(sizeof(E_Buffer));
    buffer->usage = E_BufferUsageIndex;

#ifdef EUPHORBE_WINDOWS
    buffer->rhi_handle = E_Vk_AllocateIndexBuffer(size);
#endif

    return buffer;
}

E_Buffer* E_CreateUniformBuffer(i64 size)
{
    E_Buffer* buffer = malloc(sizeof(E_Buffer));
    buffer->usage = E_BufferUsageUniform;

#ifdef EUPHORBE_WINDOWS
    buffer->rhi_handle = E_Vk_AllocateUniformBuffer(size);
#endif

    return buffer;
}

void E_SetBufferData(E_Buffer* buffer, void* data, i64 size)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_SetBufferData((E_VulkanBuffer*)buffer->rhi_handle, data, size);
#endif
}

void E_FreeBuffer(E_Buffer* buffer)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_FreeBuffer((E_VulkanBuffer*)buffer->rhi_handle);
#endif
    free(buffer);
}