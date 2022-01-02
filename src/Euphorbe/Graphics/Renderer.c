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

void E_RendererDrawMemoryUsageGUI()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_DrawMemoryUsageGUI();
#endif
}

E_Image* E_GetSwapchainImage()
{
#ifdef EUPHORBE_WINDOWS
    return E_Vk_GetSwapchainImage();
#endif
}

u32 E_GetSwapchainImageIndex()
{
#ifdef EUPHORBE_WINDOWS
    return E_Vk_GetSwapchainImageIndex();
#endif
}

E_CommandBuffer* E_GetSwapchainCommandBuffer()
{
#ifdef EUPHORBE_WINDOWS
    return E_Vk_GetSwapchainCommandBuffer();
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

void E_RendererResize(i32 width, i32 height)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_Resize(width, height);
#endif
}