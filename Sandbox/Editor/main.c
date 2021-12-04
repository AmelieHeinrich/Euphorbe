#include <Euphorbe/Euphorbe.h>

E_Window* window;

int main()
{
    window = E_CreateWindow(1280, 720, "Euphorbe Editor");
    E_RendererInit(window);

    E_Image* depth_image = E_MakeImage(1280, 720, E_ImageFormatD32_Float);

    E_LaunchWindow(window);

    while (E_IsWindowOpen(window))
    {
        E_WindowUpdate(window);

        E_RendererBegin();

        // Begin depth image transition
        E_ImageTransitionLayout(depth_image, 
                                0, E_ImageAccessDepthWrite, 
                                E_ImageLayoutUndefined, E_ImageLayoutDepth, 
                                E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment, 
                                E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment);

        E_ClearValue depth_clear = { 0 };

        E_ImageAttachment attachments[] = {
            { depth_image, E_ImageLayoutDepth, depth_clear }
        };

        E_RendererStartRender(attachments, 1, 1);
        E_RendererEndRender();

        E_RendererEnd();
    }

    E_RendererWait();

    E_FreeImage(depth_image);
    E_RendererShutdown();
    E_FreeWindow(window);
    return 0;
}