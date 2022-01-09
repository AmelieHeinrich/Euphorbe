#ifndef EUPHORBE_VULKAN_BUFFER_H
#define EUPHORBE_VULKAN_BUFFER_H

#include <Euphorbe/Graphics/Buffer.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

typedef struct E_VulkanBuffer E_VulkanBuffer;
struct E_VulkanBuffer
{
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;
};

E_VulkanBuffer* E_Vk_AllocateVertexBuffer(i64 size);
E_VulkanBuffer* E_Vk_AllocateIndexBuffer(i64 size);
E_VulkanBuffer* E_Vk_AllocateUniformBuffer(i64 size);
E_VulkanBuffer* E_Vk_AllocateStorageBuffer(i64 size);

void E_Vk_SetBufferData(E_VulkanBuffer* buffer, void* data, i64 size);
void E_Vk_FreeBuffer(E_VulkanBuffer* buffer);

#endif