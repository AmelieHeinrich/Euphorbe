#include <Euphorbe/Euphorbe.h>

E_Window* window;
E_Image* depth_image;
E_Image* swapchain_buffer;

void BeginRender()
{
    E_RendererBegin();
    swapchain_buffer = E_GetSwapchainImage();

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
}

void EndRender()
{
    E_ImageTransitionLayout(swapchain_buffer,
        E_ImageAccessColorWrite, 0,
        E_ImageLayoutColor, E_ImageLayoutSwapchainPresent,
        E_ImagePipelineStageColorOutput,
        E_ImagePipelineStageBottom);
    E_RendererEnd();
    E_WindowUpdate(window);
}

void ResizeCallback(i32 width, i32 height)
{
    E_RendererResize(width, height);
    E_ImageResize(depth_image, width, height);
}

int main()
{
    E_RendererInitSettings settings = { 0 };
    settings.gpu_pool_size = MEGABYTES(32);
    settings.log_found_layers = 0;
    settings.log_renderer_events = 1;

    window = E_CreateWindow(1280, 720, "Euphorbe Editor");
    E_RendererInit(window, settings);
    depth_image = E_MakeImage(1280, 720, E_ImageFormatD32_Float);
    E_LaunchWindow(window);

    E_WindowSetResizeCallback(window, ResizeCallback);

    while (E_IsWindowOpen(window))
    {
        BeginRender();

        E_ClearValue color_clear = { 0.1f, 0.2f, 0.3f, 1.0f, 0, 0 };
        E_ClearValue depth_clear = { 0 };

        E_ImageAttachment attachments[2] = {
            { swapchain_buffer, E_ImageLayoutColor, color_clear },
            { depth_image, E_ImageLayoutDepth, depth_clear }
        };

        // Render loop

        E_RendererStartRender(attachments, 2, 1);
        E_RendererEndRender();

        //

        EndRender();
    }

    E_RendererWait();
    E_FreeImage(depth_image);
    E_RendererShutdown();
    E_FreeWindow(window);
    return 0;
}