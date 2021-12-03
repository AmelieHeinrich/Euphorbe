#include "Renderer.h"

#ifdef EUPHORBE_WINDOWS
    #include "Vulkan/VulkanRenderer.h"
#endif

void E_RendererInit(E_Window* window)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_RendererInit(window);
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