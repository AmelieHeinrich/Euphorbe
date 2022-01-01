#ifndef EUPHORBE_VULKAN_COMMAND_BUFFER
#define EUPHORBE_VULKAN_COMMAND_BUFFER

#include <Euphorbe/Graphics/CommandBuffer.h>
#include <Euphorbe/Graphics/Renderer.h>
#include <volk.h>

#include "VulkanMaterial.h"
#include "VulkanBuffer.h"

typedef struct E_VulkanCommandBuffer E_VulkanCommandBuffer;
struct E_VulkanCommandBuffer
{
	E_CommandBufferType type;
	VkCommandPool* pool_ptr;
	VkCommandBuffer handle;
};

E_VulkanCommandBuffer* E_Vk_CreateCommandBuffer(E_CommandBufferType type);
void E_Vk_FreeCommandBuffer(E_VulkanCommandBuffer* buffer);
void E_Vk_SubmitCommandBuffer(E_VulkanCommandBuffer* buffer);

E_VulkanCommandBuffer* E_Vk_CreateUploadCommandBuffer();
void E_Vk_SubmitUploadCommandBuffer(E_VulkanCommandBuffer* buffer);

void E_Vk_BeginCommandBuffer(E_VulkanCommandBuffer* cmd);
void E_Vk_EndCommandBuffer(E_VulkanCommandBuffer* cmd);
void E_Vk_CommandBufferSetViewport(E_VulkanCommandBuffer* cmd, i32 width, i32 height);
void E_Vk_CommandBufferBindMaterial(E_VulkanCommandBuffer* cmd, E_VulkanMaterial* material);
void E_Vk_CommandBufferBindComputeMaterial(E_VulkanCommandBuffer* cmd, E_VulkanMaterial* material);
void E_Vk_CommandBufferBindBuffer(E_VulkanCommandBuffer* cmd, E_VulkanBuffer* buffer, E_BufferUsage usage);
void E_Vk_CommandBufferBindMaterialInstance(E_VulkanCommandBuffer* cmd, E_VulkanMaterialInstance* instance, E_VulkanMaterial* material, i32 set_index);
void E_Vk_CommandBufferBindComputeMaterialInstance(E_VulkanCommandBuffer* cmd, E_VulkanMaterialInstance* instance, E_VulkanMaterial* material, i32 set_index);
void E_Vk_CommandBufferDraw(E_VulkanCommandBuffer* cmd, u32 first, u32 count);
void E_Vk_CommandBufferDrawIndexed(E_VulkanCommandBuffer* cmd, u32 first, u32 count);
void E_Vk_CommandBufferDispatch(E_VulkanCommandBuffer* cmd, u32 groupX, u32 groupY, u32 groupZ);
void E_Vk_CommandBufferStartRender(E_VulkanCommandBuffer* cmd, E_ImageAttachment* attachments, i32 attachment_count, vec2 render_size, b32 has_depth);
void E_Vk_CommandBufferEndRender(E_VulkanCommandBuffer* cmd);
void E_Vk_CommandBufferPushConstants(E_VulkanCommandBuffer* cmd_buf, E_VulkanMaterial* material, void* data, i64 size);
void E_Vk_CommandBufferBlitImage(E_VulkanCommandBuffer* cmd_buf, E_VulkanImage* src, E_VulkanImage* dst, E_ImageLayout src_layout, E_ImageLayout dst_layout);
void E_Vk_CommandBufferImageTransitionLayout(E_VulkanCommandBuffer* cmd_buf, E_VulkanImage* image, E_ImageAccess srcAccess, E_ImageAccess dstAccess, E_ImageLayout old, E_ImageLayout new, E_ImagePipelineStage srcStage, E_ImagePipelineStage dstStage, u32 layer);

#endif