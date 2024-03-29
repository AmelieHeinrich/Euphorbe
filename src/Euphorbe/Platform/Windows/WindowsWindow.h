#pragma once

#include <Euphorbe/Platform/Window.h>

// Struct containing all the necessary information to create a Win32 window
typedef struct E_WindowsWindow E_WindowsWindow;
struct E_WindowsWindow
{
    HWND hwnd;
    b32 is_open;
    i32* width_pointer;
    i32* height_pointer;

    E_WindowResizeCallback resize_callback;
    E_WindowScrollCallback scroll_callback;
};

E_WindowsWindow* E_CreateWindowsWindow(i32* width, i32* height, const char* title, b32 dark_mode);
void E_LaunchWindowsWindow(E_WindowsWindow* window);
void E_FreeWindowsWindow(E_WindowsWindow* window);
void E_UpdateWindowsWindow(E_WindowsWindow* window);