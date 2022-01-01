#ifndef EUPHORBE_RENDERER_H
#define EUPHORBE_RENDERER_H

#include <Euphorbe/Core/Log.h>
#include <Euphorbe/Platform/Window.h>
#include <cglm/cglm.h>

#include "Image.h"
#include "Material.h"
#include "Buffer.h"

typedef struct E_CommandBuffer E_CommandBuffer;

typedef struct E_RendererInitSettings E_RendererInitSettings;
struct E_RendererInitSettings
{
    b32 log_found_layers; // Default: true
    b32 log_renderer_events; // Default: true
    b32 enable_debug; // Default: false
    b32 gui_should_clear; // If viewport is in a gui window
};

typedef struct E_ClearValue E_ClearValue;
struct E_ClearValue
{
    f32 r;
    f32 g;
    f32 b;
    f32 a;
    f32 depth;
    u32 stencil;
};

typedef struct E_ImageAttachment E_ImageAttachment;
struct E_ImageAttachment
{
    E_Image* image;
    E_ImageLayout layout;
    E_ClearValue clear_value;
};

// Light info
typedef struct E_PointLight E_PointLight;
struct E_PointLight
{
    vec4 position;
    vec4 color;
};

void E_RendererInit(E_Window* window, E_RendererInitSettings settings);
void E_RendererShutdown();

void E_RendererBegin();
void E_RendererEnd();
void E_RendererWait();

E_Image* E_GetSwapchainImage();
u32 E_GetSwapchainImageIndex();
E_CommandBuffer* E_GetSwapchainCommandBuffer();

//
void E_BeginGUI();
void E_EndGUI();

void E_RendererResize(i32 width, i32 height);

#endif