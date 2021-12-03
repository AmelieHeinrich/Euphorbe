#include "Window.h"

#ifdef EUPHORBE_WINDOWS
    #include "Windows/WindowsWindow.h"
#endif

E_Window* E_CreateWindow(i32 width, i32 height, const char* title)
{
    assert(width != 0 && height != 0);

    E_Window* window = malloc(sizeof(E_Window));
    window->width = width;
    window->height = height;
    window->title = title;

#ifdef EUPHORBE_WINDOWS
    window->platform_data = E_CreateWindowsWindow(&window->width, &window->height, title);
#endif

    return window;
}

void E_FreeWindow(E_Window* window)
{
#ifdef EUPHORBE_WINDOWS
    E_FreeWindowsWindow(window->platform_data);
#endif

    free(window);
}

b32 E_IsWindowOpen(E_Window* window)
{
#ifdef EUPHORBE_WINDOWS
    E_WindowsWindow* win32 = (E_WindowsWindow*)window->platform_data;
    return win32->is_open;
#endif
}

void E_WindowUpdate(E_Window* window)
{
#ifdef EUPHORBE_WINDOWS
    E_UpdateWindowsWindow(window->platform_data, &window->width, &window->height);
#endif
}