#ifndef EUPHORBE_RENDERER_H
#define EUPHORBE_RENDERER_H

#include <Euphorbe/Core/Log.h>
#include <Euphorbe/Platform/Window.h>
#include "Image.h"

typedef struct E_ClearValue E_ClearValue;
struct E_ClearValue
{
    f32 r;
    f32 g;
    f32 b;
    f32 a;
    u32 depth;
    u32 stencil;
};

typedef struct E_ImageAttachment E_ImageAttachment;
struct E_ImageAttachment
{
    E_Image* image;
    E_ImageLayout layout;
    E_ClearValue clear_value;
};

void E_RendererInit(E_Window* window);
void E_RendererShutdown();

void E_RendererBegin();
void E_RendererEnd();
void E_RendererWait();

// Put all of your color + depth attachments in the array. Depth attachment must be at the end
void E_RendererStartRender(E_ImageAttachment* attachments, i32 attachment_count, i32 has_depth);
void E_RendererEndRender();
E_Image* E_GetSwapchainImage();

void E_RendererResize(i32 width, i32 height);

#endif