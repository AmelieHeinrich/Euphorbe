#ifndef EUPHORBE_WINDOW_H
#define EUPHORBE_WINDOW_H

#include <Euphorbe/Core/Common.h>

typedef struct E_Window E_Window;
struct E_Window
{
    u32 width;
    u32 height;
    const char* title;
    // Platform handle for the window. See E_WindowsWindow (Euphorbe/Platform/Windows/WindowsWindow.h) for more details
    void* platform_data;
};

E_Window* E_CreateWindow(i32 width, i32 height, const char* title);
void E_LaunchWindow(E_Window* window);
void E_FreeWindow(E_Window* window);
b32 E_IsWindowOpen(E_Window* window);
void E_WindowUpdate(E_Window* window);

#endif