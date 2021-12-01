#include <Euphorbe/Euphorbe.h>

E_Window* window;

int main()
{
    window = E_CreateWindow(1280, 720, "Euphorbe Editor");

    while (E_IsWindowOpen(window))
    {
        E_WindowUpdate(window);
    }

    E_FreeWindow(window);
    return 0;
}