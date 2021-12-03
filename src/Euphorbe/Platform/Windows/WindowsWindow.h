#ifndef EUPHORBE_WINDOWS_WINDOW_H
#define EUPHORBE_WINDOWS_WINDOW_H

#include <Euphorbe/Platform/Window.h>

// Struct containing all the necessary information to create a Win32 window
typedef struct E_WindowsWindow E_WindowsWindow;
struct E_WindowsWindow
{
    HWND hwnd;
    b32 is_open;
};

E_WindowsWindow* E_CreateWindowsWindow(i32* width, i32* height, const char* title);
void E_FreeWindowsWindow(E_WindowsWindow* window);
void E_UpdateWindowsWindow(E_WindowsWindow* window, i32* width, i32* height);

#endif