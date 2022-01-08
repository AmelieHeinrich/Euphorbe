#include "CommandBuffer.h"

#ifdef EUPHORBE_WINDOWS
	#include <Euphorbe/Graphics/Vulkan/VulkanCommandBuffer.h>
#endif

#include "Renderer.h"

E_CommandBuffer* E_CreateCommandBuffer(E_CommandBufferType type)
{
	E_CommandBuffer* cmd_buf = malloc(sizeof(E_CommandBuffer));
	cmd_buf->type = type;

#ifdef EUPHORBE_WINDOWS
	cmd_buf->rhi_handle = E_Vk_CreateCommandBuffer(type);
#endif
	return cmd_buf;
}

void E_FreeCommandBuffer(E_CommandBuffer* buffer)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_FreeCommandBuffer(buffer->rhi_handle);
#endif
}

void E_SubmitCommandBuffer(E_CommandBuffer* buffer)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_SubmitCommandBuffer(buffer->rhi_handle);
#endif
}

void E_BeginCommandBuffer(E_CommandBuffer* cmd)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_BeginCommandBuffer(cmd->rhi_handle);
#endif
}

void E_EndCommandBuffer(E_CommandBuffer* cmd)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_EndCommandBuffer(cmd->rhi_handle);
#endif
}

void E_CommandBufferSetViewport(E_CommandBuffer* cmd, i32 width, i32 height)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferSetViewport(cmd->rhi_handle, width, height);
#endif
}

void E_CommandBufferBindMaterial(E_CommandBuffer* cmd, E_Material* material)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferBindMaterial(cmd->rhi_handle, material->rhi_handle);
#endif
}

void E_CommandBufferBindComputeMaterial(E_CommandBuffer* cmd, E_Material* material)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferBindComputeMaterial(cmd->rhi_handle, material->rhi_handle);
#endif
}

void E_CommandBufferBindBuffer(E_CommandBuffer* cmd, E_Buffer* buffer)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferBindBuffer(cmd->rhi_handle, buffer->rhi_handle, buffer->usage);
#endif
}

void E_CommandBufferBindMaterialInstance(E_CommandBuffer* cmd, E_MaterialInstance* instance, E_Material* material, i32 set_index)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferBindMaterialInstance((E_VulkanCommandBuffer*)cmd->rhi_handle, (E_VulkanMaterialInstance*)instance->rhi_handle, (E_VulkanMaterial*)material->rhi_handle, set_index);
#endif
}

void E_CommandBufferBindComputeMaterialInstance(E_CommandBuffer* cmd, E_MaterialInstance* instance, E_Material* material, i32 set_index)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferBindComputeMaterialInstance(cmd->rhi_handle, instance->rhi_handle, material->rhi_handle, set_index);
#endif
}

void E_CommandBufferDraw(E_CommandBuffer* cmd, u32 first, u32 count)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferDraw(cmd->rhi_handle, first, count);
#endif

	E_CurrentRendererStatistics.total_draw_calls += 1;
	E_CurrentRendererStatistics.total_vertex_count += count;
	E_CurrentRendererStatistics.total_index_count += count / 6;
	E_CurrentRendererStatistics.total_triangle_count += count / 3;
}

void E_CommandBufferDrawIndexed(E_CommandBuffer* cmd, u32 first, u32 count)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferDrawIndexed(cmd->rhi_handle, first, count);
#endif

	E_CurrentRendererStatistics.total_draw_calls += 1;
	E_CurrentRendererStatistics.total_vertex_count += count * 6;
	E_CurrentRendererStatistics.total_index_count += count;
	E_CurrentRendererStatistics.total_triangle_count += count / 3;
}

void E_CommandBufferDrawMeshlets(E_CommandBuffer* cmd, u32 first, u32 count)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferDrawMeshlets(cmd->rhi_handle, first, count);
#endif

	E_CurrentRendererStatistics.total_draw_calls += 1;
	E_CurrentRendererStatistics.total_meshlet_count += count;
}

void E_CommandBufferDispatch(E_CommandBuffer* cmd, u32 groupX, u32 groupY, u32 groupZ)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferDispatch(cmd->rhi_handle, groupX, groupY, groupZ);
#endif
}

void E_CommandBufferStartRender(E_CommandBuffer* cmd, E_ImageAttachment* attachments, i32 attachment_count, vec2 render_size, b32 has_depth)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferStartRender(cmd->rhi_handle, attachments, attachment_count, render_size, has_depth);
#endif
}

void E_CommandBufferEndRender(E_CommandBuffer* cmd)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferEndRender(cmd->rhi_handle);
#endif
}

void E_CommandBufferPushConstants(E_CommandBuffer* cmd_buf, E_Material* material, void* data, i64 size)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferPushConstants(cmd_buf->rhi_handle, material->rhi_handle, data, size);
#endif
}

void E_CommandBufferBlitImage(E_CommandBuffer* cmd_buf, E_Image* src, E_Image* dst, E_ImageLayout src_layout, E_ImageLayout dst_layout)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferBlitImage(cmd_buf->rhi_handle, src->rhi_handle, dst->rhi_handle, src_layout, dst_layout);
#endif
}

void E_CommandBufferImageTransitionLayout(E_CommandBuffer* cmd_buf, E_Image* image, E_ImageAccess srcAccess, E_ImageAccess dstAccess, E_ImageLayout old, E_ImageLayout new, E_ImagePipelineStage srcStage, E_ImagePipelineStage dstStage, u32 layer)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_CommandBufferImageTransitionLayout(cmd_buf->rhi_handle, image->rhi_handle, srcAccess, dstAccess, old, new, srcStage, dstStage, layer);
#endif
}
