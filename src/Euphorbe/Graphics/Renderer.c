#include "Renderer.h"

#ifdef EUPHORBE_WINDOWS
    #include "Vulkan/VulkanRenderer.h"
#endif

E_RendererStats E_CurrentRendererStatistics;

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
    E_CurrentRendererStatistics.total_draw_calls = 0;
    E_CurrentRendererStatistics.total_index_count = 0;
    E_CurrentRendererStatistics.total_vertex_count = 0;
    E_CurrentRendererStatistics.total_triangle_count = 0;
    E_CurrentRendererStatistics.total_meshlet_count = 0;

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

void E_RendererPresent()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_Present();
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

void E_RendererDrawRendererStats()
{
    igText("Vertices: %d", E_CurrentRendererStatistics.total_vertex_count);
    igText("Indices: %d", E_CurrentRendererStatistics.total_index_count);
    igText("Triangles: %d", E_CurrentRendererStatistics.total_triangle_count);
    igText("Meshlets: %d", E_CurrentRendererStatistics.total_meshlet_count);
    igText("Draw Calls: %d", E_CurrentRendererStatistics.total_draw_calls);
}

void E_RendererDrawGraphicsCardInfo()
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_DrawGraphicsCardInfo();
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