#ifndef EUPHORBE_WINDOW_H
#define EUPHORBE_WINDOW_H

#include <Euphorbe/Core/Common.h>

typedef void (*E_WindowResizeCallback)(int, int);
typedef void (*E_WindowScrollCallback)(f32);

typedef struct E_Window E_Window;
struct E_Window
{
    u32 width;
    u32 height;
    const char* title;
    // Platform handle for the window. See E_WindowsWindow (Euphorbe/Platform/Windows/WindowsWindow.h) for more details
    void* platform_data;
};

E_Window* E_CreateWindow(i32 width, i32 height, const char* title, b32 dark_mode);
void E_LaunchWindow(E_Window* window);
void E_FreeWindow(E_Window* window);
b32 E_IsWindowOpen(E_Window* window);
void E_WindowUpdate(E_Window* window);

// Callbacks
void E_WindowSetResizeCallback(E_Window* window, E_WindowResizeCallback callback);
void E_WindowSetScrollCallback(E_Window* window, E_WindowScrollCallback callback);

#endif