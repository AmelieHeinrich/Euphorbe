#include "Renderer.h"

#ifdef EUPHORBE_WINDOWS
    #include "Vulkan/VulkanRenderer.h"
#endif

void E_RendererInit(E_Window* window, E_RendererInitSettings settings)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_RendererInit(window, settings);
#endif
}

void E_RendererShutdown()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_RendererShutdown();
#endif
}

void E_RendererBegin()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_Begin();
#endif
}

void E_RendererEnd()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_End();
#endif
}

void E_RendererWait()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_DeviceWait();
#endif
}

void E_RendererStartRender(E_ImageAttachment* attachments, i32 attachment_count, i32 has_depth)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_RendererStartRender(attachments, attachment_count, has_depth);
#endif
}

void E_RendererEndRender()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_RendererEndRender();
#endif
}

E_Image* E_GetSwapchainImage()
{
#ifdef EUPHORBE_WINDOWS
    return E_Vk_GetSwapchainImage();
#endif
}

void E_BeginGUI()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_BeginGUI();
#endif
}

void E_EndGUI()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_EndGUI();
#endif
}

void E_BindMaterial(E_Material* material)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_BindMaterial((E_VulkanMaterial*)material->rhi_handle);
#endif
}

void E_BindBuffer(E_Buffer* buffer)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_BindBuffer((E_VulkanBuffer*)buffer->rhi_handle, buffer->usage);
#endif
}

void E_Draw(u32 first, u32 count)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_Draw(first, count);
#endif
}

void E_DrawIndexed(u32 first, u32 count)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_DrawIndexed(first, count);
#endif
}

void E_RendererResize(i32 width, i32 height)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_Resize(width, height);
#endif
}