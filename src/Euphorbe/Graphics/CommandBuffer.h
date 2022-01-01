#ifndef EUPHORBE_COMMAND_BUFFER_H
#define EUPHORBE_COMMAND_BUFFER_H

#include "Buffer.h"
#include "Material.h"
#include "Renderer.h"

typedef enum E_CommandBufferType E_CommandBufferType;
enum E_CommandBufferType
{
	E_CommandBufferTypeGraphics,
	E_CommandBufferTypeCompute
};

typedef struct E_CommandBuffer E_CommandBuffer;
struct E_CommandBuffer
{
	E_CommandBufferType type;
	void* rhi_handle;
};

E_CommandBuffer* E_CreateCommandBuffer(E_CommandBufferType type);
void E_FreeCommandBuffer(E_CommandBuffer* buffer);
E_CommandBuffer* E_BeginSingleTimeCommands(E_CommandBufferType type);
void E_EndSingleTimeCommands(E_CommandBuffer* buffer);

//
void E_BeginCommandBuffer(E_CommandBuffer* cmd);
void E_EndCommandBuffer(E_CommandBuffer* cmd);

//
void E_CommandBufferSetViewport(E_CommandBuffer* cmd, i32 width, i32 height);
void E_CommandBufferBindMaterial(E_CommandBuffer* cmd, E_Material* material);
void E_CommandBufferBindBuffer(E_CommandBuffer* cmd, E_Buffer* buffer);
void E_CommandBufferBindMaterialInstance(E_CommandBuffer* cmd, E_MaterialInstance* instance, E_Material* material, i32 set_index);
void E_CommandBufferDraw(E_CommandBuffer* cmd, u32 first, u32 count);
void E_CommandBufferDrawIndexed(E_CommandBuffer* cmd, u32 first, u32 count);
void E_CommandBufferStartRender(E_CommandBuffer* cmd, E_ImageAttachment* attachments, i32 attachment_count, vec2 render_size, b32 has_depth);
void E_CommandBufferEndRender(E_CommandBuffer* cmd);
void E_CommandBufferPushConstants(E_CommandBuffer* cmd_buf, E_Material* material, void* data, i64 size);
void E_CommandBufferBlitImage(E_CommandBuffer* cmd_buf, E_Image* src, E_Image* dst, E_ImageLayout src_layout, E_ImageLayout dst_layout);
void E_CommandBufferImageTransitionLayout(E_CommandBuffer* cmd_buf, E_Image* image, E_ImageAccess srcAccess, E_ImageAccess dstAccess, E_ImageLayout old, E_ImageLayout new, E_ImagePipelineStage srcStage, E_ImagePipelineStage dstStage);

#endif