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
        E_RendererBegin();
        E_Image* swapchain_buffer = E_GetSwapchainImage();

        // Setup image transition
        E_ImageTransitionLayout(swapchain_buffer,
                                0, E_ImageAccessColorWrite,
                                E_ImageLayoutUndefined, E_ImageLayoutColor,
                                E_ImagePipelineStageTop,
                                E_ImagePipelineStageColorOutput);

        E_ImageTransitionLayout(depth_image, 
                                0, E_ImageAccessDepthWrite, 
                                E_ImageLayoutUndefined, E_ImageLayoutDepth, 
                                E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment, 
                                E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment);

        E_ClearValue color_clear = { 0.1f, 0.2f, 0.3f, 1.0f, 0.0f, 0.0f };
        E_ClearValue depth_clear = { 0 };

        E_ImageAttachment attachments[2] = {
            { swapchain_buffer, E_ImageLayoutColor, color_clear },
            { depth_image, E_ImageLayoutDepth, depth_clear }
        };

        // Render loop

        E_RendererStartRender(attachments, 2, 1);
        E_RendererEndRender();

        //

        E_ImageTransitionLayout(swapchain_buffer,
                                E_ImageAccessColorWrite, 0,
                                E_ImageLayoutColor, E_ImageLayoutSwapchainPresent,
                                E_ImagePipelineStageColorOutput,
                                E_ImagePipelineStageBottom);
        E_RendererEnd();
        E_WindowUpdate(window);
    }

    E_RendererWait();

    E_FreeImage(depth_image);
    E_RendererShutdown();
    E_FreeWindow(window);
    return 0;
}