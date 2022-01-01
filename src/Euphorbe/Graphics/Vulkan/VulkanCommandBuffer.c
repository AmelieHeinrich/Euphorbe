#include "VulkanCommandBuffer.h"

#include "VulkanRenderer.h"

#define GET_CMD_BUF_POOL(type) type == E_CommandBufferTypeGraphics ? rhi.command.graphics_command_pool : rhi.command.compute_command_pool
#define GET_CMD_BUF_POOL_PTR(type) type == E_CommandBufferTypeGraphics ? &rhi.command.graphics_command_pool : &rhi.command.compute_command_pool
#define GET_CMD_BUF_QUEUE(type) type == E_CommandBufferTypeGraphics ? rhi.device.graphics_queue : rhi.device.compute_queue

E_VulkanCommandBuffer* E_Vk_CreateCommandBuffer(E_CommandBufferType type)
{
    E_VulkanCommandBuffer* cmd_buf = malloc(sizeof(E_VulkanCommandBuffer));
    cmd_buf->type = type;
    cmd_buf->pool_ptr = GET_CMD_BUF_POOL_PTR(type);

    VkCommandBufferAllocateInfo alloc_info = { 0 };
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = GET_CMD_BUF_POOL(type);
    alloc_info.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(rhi.device.handle, &alloc_info, &cmd_buf->handle);
    assert(result == VK_SUCCESS);

    return cmd_buf;
}

void E_Vk_FreeCommandBuffer(E_VulkanCommandBuffer* buffer)
{
    free(buffer);
}

void E_Vk_SubmitCommandBuffer(E_VulkanCommandBuffer* buffer)
{
    VkQueue submit_queue = GET_CMD_BUF_QUEUE(buffer->type);

    VkResult result = vkEndCommandBuffer(buffer->handle);
    assert(result == VK_SUCCESS);

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer->handle;

    vkQueueSubmit(submit_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(submit_queue);

    if (buffer->type == E_CommandBufferTypeCompute)
        vkResetCommandPool(rhi.device.handle, rhi.command.compute_command_pool, 0);
}

E_VulkanCommandBuffer* E_Vk_CreateUploadCommandBuffer()
{
    E_VulkanCommandBuffer* cmd_buf = malloc(sizeof(E_VulkanCommandBuffer));
    cmd_buf->type = E_CommandBufferTypeGraphics;
    cmd_buf->pool_ptr = &rhi.command.upload_command_pool;

    VkCommandBufferAllocateInfo alloc_info = { 0 };
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandPool = rhi.command.upload_command_pool;
    alloc_info.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(rhi.device.handle, &alloc_info, &cmd_buf->handle);
    assert(result == VK_SUCCESS);

    E_Vk_BeginCommandBuffer(cmd_buf);

    return cmd_buf;
}

void E_Vk_SubmitUploadCommandBuffer(E_VulkanCommandBuffer* buffer)
{
    E_Vk_EndCommandBuffer(buffer);

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &buffer->handle;

    vkQueueSubmit(rhi.device.graphics_queue, 1, &submit_info, rhi.command.upload_fence);
    vkWaitForFences(rhi.device.handle, 1, &rhi.command.upload_fence, VK_TRUE, INT_MAX);
    vkResetFences(rhi.device.handle, 1, &rhi.command.upload_fence);
    vkResetCommandPool(rhi.device.handle, rhi.command.upload_command_pool, 0);

    free(buffer);
}

void E_Vk_BeginCommandBuffer(E_VulkanCommandBuffer* cmd)
{
    VkCommandBufferBeginInfo begin_info = { 0 };
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkResult result = vkBeginCommandBuffer(cmd->handle, &begin_info);
    assert(result == VK_SUCCESS);
}

void E_Vk_EndCommandBuffer(E_VulkanCommandBuffer* cmd)
{
    VkResult result = vkEndCommandBuffer(cmd->handle);
    assert(result == VK_SUCCESS);
}

void E_Vk_CommandBufferSetViewport(E_VulkanCommandBuffer* cmd, i32 width, i32 height)
{
    VkViewport viewport = { 0 };
    viewport.width = (f32)width;
    viewport.height = (f32)height;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { 0 };
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = (u32)width;
    scissor.extent.height = (u32)height;

    vkCmdSetViewport(cmd->handle, 0, 1, &viewport);
    vkCmdSetScissor(cmd->handle, 0, 1, &scissor);
}

void E_Vk_CommandBufferBindMaterial(E_VulkanCommandBuffer* cmd, E_VulkanMaterial* material)
{
    vkCmdBindPipeline(cmd->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline);
}

void E_Vk_CommandBufferBindComputeMaterial(E_VulkanCommandBuffer* cmd, E_VulkanMaterial* material)
{
    vkCmdBindPipeline(cmd->handle, VK_PIPELINE_BIND_POINT_COMPUTE, material->pipeline);
}

void E_Vk_CommandBufferBindBuffer(E_VulkanCommandBuffer* cmd, E_VulkanBuffer* buffer, E_BufferUsage usage)
{
    switch (usage)
    {
    case E_BufferUsageVertex:
        VkDeviceSize offsets[] = { 0 };
        VkBuffer buffers[] = { buffer->buffer };
        vkCmdBindVertexBuffers(cmd->handle, 0, 1, buffers, offsets);
        break;
    case E_BufferUsageIndex:
        vkCmdBindIndexBuffer(cmd->handle, buffer->buffer, 0, VK_INDEX_TYPE_UINT32);
        break;
    default:
        return;
    }
}

void E_Vk_CommandBufferBindMaterialInstance(E_VulkanCommandBuffer* cmd, E_VulkanMaterialInstance* instance, E_VulkanMaterial* material, i32 set_index)
{
    vkCmdBindDescriptorSets(cmd->handle, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline_layout, set_index, 1, &instance->set, 0, NULL);
}

void E_Vk_CommandBufferBindComputeMaterialInstance(E_VulkanCommandBuffer* cmd, E_VulkanMaterialInstance* instance, E_VulkanMaterial* material, i32 set_index)
{
    vkCmdBindDescriptorSets(cmd->handle, VK_PIPELINE_BIND_POINT_COMPUTE, material->pipeline_layout, set_index, 1, &instance->set, 0, NULL);
}

void E_Vk_CommandBufferDraw(E_VulkanCommandBuffer* cmd, u32 first, u32 count)
{
    vkCmdDraw(cmd->handle, count, 1, first, 0);
}

void E_Vk_CommandBufferDrawIndexed(E_VulkanCommandBuffer* cmd, u32 first, u32 count)
{
    vkCmdDrawIndexed(cmd->handle, count, 1, 0, 0, 0);
}

void E_Vk_CommandBufferDispatch(E_VulkanCommandBuffer* cmd, u32 groupX, u32 groupY, u32 groupZ)
{
    vkCmdDispatch(cmd->handle, groupX, groupY, groupZ);
}

#pragma optimize("", off)
void E_Vk_CommandBufferStartRender(E_VulkanCommandBuffer* cmd, E_ImageAttachment* attachments, i32 attachment_count, vec2 render_size, b32 has_depth)
{
    u32 color_iterator = has_depth ? attachment_count - 1 : attachment_count;

    VkRect2D render_area = { 0 };
    render_area.extent.width = (u32)render_size[0];
    render_area.extent.height = (u32)render_size[1];
    render_area.offset.x = 0;
    render_area.offset.y = 0;

    VkRenderingInfoKHR rendering_info = { 0 };
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea = render_area;
    rendering_info.colorAttachmentCount = color_iterator;
    rendering_info.layerCount = 1;

    // Max attachment count is 64
    VkRenderingAttachmentInfoKHR color_attachments[64] = { 0 };

    for (u32 i = 0; i < color_iterator; i++)
    {
        E_VulkanImage* vk_image = (E_VulkanImage*)attachments[i].image->rhi_handle;

        VkClearValue clear_value = { 0 };
        clear_value.color.float32[0] = attachments[i].clear_value.r;
        clear_value.color.float32[1] = attachments[i].clear_value.g;
        clear_value.color.float32[2] = attachments[i].clear_value.b;
        clear_value.color.float32[3] = attachments[i].clear_value.a;

        VkRenderingAttachmentInfoKHR color_attachment_info = { 0 };
        color_attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        color_attachment_info.imageView = vk_image->image_view;
        color_attachment_info.imageLayout = attachments[i].layout;
        color_attachment_info.resolveMode = VK_RESOLVE_MODE_NONE;
        color_attachment_info.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment_info.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment_info.clearValue = clear_value;

        color_attachments[i] = color_attachment_info;
    }

    if (has_depth)
    {
        E_VulkanImage* vk_image = (E_VulkanImage*)attachments[color_iterator].image->rhi_handle;

        VkClearValue depth_clear_value = { 0 };
        depth_clear_value.depthStencil.depth = 1.0f;
        depth_clear_value.depthStencil.stencil = attachments[color_iterator].clear_value.stencil;

        VkRenderingAttachmentInfoKHR depth_attachment = { 0 };
        depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depth_attachment.imageView = vk_image->image_view;
        depth_attachment.imageLayout = attachments[color_iterator].layout;
        depth_attachment.resolveMode = VK_RESOLVE_MODE_NONE;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.clearValue = depth_clear_value;

        rendering_info.pStencilAttachment = &depth_attachment;
        rendering_info.pDepthAttachment = &depth_attachment;
    }

    rendering_info.pColorAttachments = color_attachments;

    vkCmdBeginRenderingKHR(cmd->handle, &rendering_info);
}
#pragma optimize("", on)

void E_Vk_CommandBufferEndRender(E_VulkanCommandBuffer* cmd)
{
    vkCmdEndRenderingKHR(cmd->handle);
}

void E_Vk_CommandBufferPushConstants(E_VulkanCommandBuffer* cmd_buf, E_VulkanMaterial* material, void* data, i64 size)
{
    vkCmdPushConstants(cmd_buf->handle, material->pipeline_layout, VK_SHADER_STAGE_ALL, 0, size, data);
}

void E_Vk_CommandBufferBlitImage(E_VulkanCommandBuffer* cmd_buf, E_VulkanImage* src, E_VulkanImage* dst, E_ImageLayout src_layout, E_ImageLayout dst_layout)
{
    VkImageBlit region = { 0 };
    region.srcOffsets[1].x = src->image_extent.width;
    region.srcOffsets[1].y = src->image_extent.height;
    region.srcOffsets[1].z = 1;
    region.srcSubresource.layerCount = 1;
    region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.dstOffsets[1].x = dst->image_extent.width;
    region.dstOffsets[1].y = dst->image_extent.height;
    region.dstOffsets[1].z = 1;
    region.dstSubresource.layerCount = 1;
    region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    vkCmdBlitImage(cmd_buf->handle, src->image, src_layout, dst->image, dst_layout, 1, &region, VK_FILTER_NEAREST);
}

void E_Vk_CommandBufferImageTransitionLayout(E_VulkanCommandBuffer* cmd_buf, E_VulkanImage* image, E_ImageAccess srcAccess, E_ImageAccess dstAccess, E_ImageLayout old, E_ImageLayout new, E_ImagePipelineStage srcStage, E_ImagePipelineStage dstStage, u32 layer)
{
    VkImageSubresourceRange range = { 0 };
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = layer;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    if (image->format == E_ImageFormatRGBA8 || image->format == E_ImageFormatRGBA16 || image->format == E_ImageFormatRGBA32 || image->format == E_ImageFormatRG16)
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    else
        range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    VkImageMemoryBarrier barrier = { 0 };
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcAccessMask = srcAccess;
    barrier.dstAccessMask = dstAccess;
    barrier.oldLayout = old;
    barrier.newLayout = new;
    barrier.image = image->image;
    barrier.subresourceRange = range;

    vkCmdPipelineBarrier(cmd_buf->handle, srcStage, dstStage, 0, 0, NULL, 0, NULL, 1, &barrier);
}
