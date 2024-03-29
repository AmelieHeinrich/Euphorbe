#include "VulkanBuffer.h"
#include "VulkanRenderer.h"

E_VulkanBuffer* E_Vk_AllocateVertexBuffer(i64 size)
{
    E_VulkanBuffer* buffer = malloc(sizeof(E_VulkanBuffer));
    
    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    VmaAllocationCreateInfo allocation_create_info = {0};
	allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VkResult result = vmaCreateBuffer(rhi.allocator, &buffer_create_info, &allocation_create_info, &buffer->buffer, &buffer->allocation, NULL);
    assert(result == VK_SUCCESS);

    return buffer;
}

E_VulkanBuffer* E_Vk_AllocateIndexBuffer(i64 size)
{
    E_VulkanBuffer* buffer = malloc(sizeof(E_VulkanBuffer));

    VkBufferCreateInfo buffer_create_info = { 0 };
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    VmaAllocationCreateInfo allocation_create_info = { 0 };
    allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VkResult result = vmaCreateBuffer(rhi.allocator, &buffer_create_info, &allocation_create_info, &buffer->buffer, &buffer->allocation, NULL);
    assert(result == VK_SUCCESS);

    return buffer;
}

E_VulkanBuffer* E_Vk_AllocateUniformBuffer(i64 size)
{
    E_VulkanBuffer* buffer = malloc(sizeof(E_VulkanBuffer));
    
    VkBufferCreateInfo buffer_create_info = {0};
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    VmaAllocationCreateInfo allocation_create_info = {0};
	allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_ONLY;

    VkResult result = vmaCreateBuffer(rhi.allocator, &buffer_create_info, &allocation_create_info, &buffer->buffer, &buffer->allocation, NULL);
    assert(result == VK_SUCCESS);

    return buffer;
}

E_VulkanBuffer* E_Vk_AllocateStorageBuffer(i64 size)
{
    E_VulkanBuffer* buffer = malloc(sizeof(E_VulkanBuffer));

    VkBufferCreateInfo buffer_create_info = { 0 };
    buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_create_info.size = size;
    buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;

    VmaAllocationCreateInfo allocation_create_info = { 0 };
    allocation_create_info.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

    VkResult result = vmaCreateBuffer(rhi.allocator, &buffer_create_info, &allocation_create_info, &buffer->buffer, &buffer->allocation, NULL);
    assert(result == VK_SUCCESS);

    return buffer;
}

void E_Vk_SetBufferData(E_VulkanBuffer* buffer, void* data, i64 size)
{
    void* buf;
    vmaMapMemory(rhi.allocator, buffer->allocation, &buf);
    memcpy(buf, data, size);
    vmaUnmapMemory(rhi.allocator, buffer->allocation);
}

void E_Vk_FreeBuffer(E_VulkanBuffer* buffer)
{
    vmaDestroyBuffer(rhi.allocator, buffer->buffer, buffer->allocation);
    free(buffer);
}