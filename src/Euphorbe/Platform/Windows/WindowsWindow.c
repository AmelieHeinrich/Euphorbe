#include "WindowsWindow.h"

#include <Euphorbe/Core/Log.h>
#include <Euphorbe/Graphics/Renderer.h>

LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_CREATE:
    {
        LPCREATESTRUCT create_struct = (LPCREATESTRUCT)lparam;
        E_WindowsWindow* window = (E_WindowsWindow*)create_struct->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);

        break;
    }
    case WM_DESTROY:
    {
        E_WindowsWindow* window = (E_WindowsWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        window->is_open = 0;
        break;
    }

    case WM_SIZE:
    {
        i32 width = LOWORD(lparam);
        i32 height = HIWORD(lparam);
        E_WindowsWindow* window = (E_WindowsWindow*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        *window->width_pointer = width;
        *window->height_pointer = height;

        if (window->resize_callback != NULL)
            window->resize_callback(width, height);

        break;
    }

    default:
        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    return 0;
}

E_WindowsWindow* E_CreateWindowsWindow(i32* width, i32* height, const char* title)
{
    E_WindowsWindow* result = malloc(sizeof(E_WindowsWindow));
    result->width_pointer = width;
    result->height_pointer = height;
    result->resize_callback = NULL;

    WNDCLASSA window_class = {0};
    window_class.lpfnWndProc = WinProc;
    window_class.hInstance = GetModuleHandleA(NULL);
    window_class.lpszClassName = "EuphorbeWindowClass";
    
    RegisterClassA(&window_class);

    result->hwnd = CreateWindowA(window_class.lpszClassName, 
                                title, 
                                WS_OVERLAPPEDWINDOW, 
                                CW_USEDEFAULT, CW_USEDEFAULT, 
                                *width, *height, 
                                NULL, NULL, window_class.hInstance, 
                                result);

    if (!result->hwnd)
        E_LogError("Failed to create HWND!");

    result->is_open = 1;

    RECT rect;
    GetClientRect(result->hwnd, &rect);
    *width = rect.right - rect.left;
    *height = rect.bottom - rect.top;

    return result;
}

void E_LaunchWindowsWindow(E_WindowsWindow* window)
{
    ShowWindow(window->hwnd, SW_SHOW);
}

void E_FreeWindowsWindow(E_WindowsWindow* window)
{
    DestroyWindow(window->hwnd);
    free(window);
}

void E_UpdateWindowsWindow(E_WindowsWindow* window)
{
    MSG msg;
    while (PeekMessageA(&msg, window->hwnd, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}