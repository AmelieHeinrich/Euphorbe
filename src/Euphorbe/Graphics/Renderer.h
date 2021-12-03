#ifndef EUPHORBE_RENDERER_H
#define EUPHORBE_RENDERER_H

#include <Euphorbe/Core/Log.h>
#include <Euphorbe/Platform/Window.h>

void E_RendererInit(E_Window* window);
void E_RendererShutdown();

void E_RendererBegin();
void E_RendererEnd();
void E_RendererWait();

// Handled automatically by the Window system. Do not call it in your program.
void E_RendererResize(i32 width, i32 height);

#endif