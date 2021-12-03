#ifndef EUPHORBE_VULKAN_RENDERER_H
#define EUPHORBE_VULKAN_RENDERER_H

#include <Euphorbe/Graphics/Renderer.h>
#include <volk.h>

typedef struct E_Vk_Data E_Vk_Data;
struct E_Vk_Data
{
    E_Window* window;

    // Instance data
    struct {
        VkInstance handle;
        char* layers[64];
        char* extensions[64];
        i32 layer_count;
        i32 extension_count;
    } instance;
};

extern E_Vk_Data rhi;

void E_Vk_RendererInit(E_Window* window);
void E_Vk_RendererShutdown();

#endif