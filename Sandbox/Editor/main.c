#include <Euphorbe/Euphorbe.h>

E_Window* window;

int main()
{
    window = E_CreateWindow(1280, 720, "Euphorbe Editor");
    E_RendererInit(window);

    while (E_IsWindowOpen(window))
    {
        E_WindowUpdate(window);

        E_RendererBegin();
        E_RendererEnd();
    }

    E_RendererWait();

    E_RendererShutdown();
    E_FreeWindow(window);
    return 0;
}