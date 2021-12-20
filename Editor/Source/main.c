#include <Euphorbe/Euphorbe.h>

#include <cimgui.h>

E_Window* window;
E_Image* depth_image;
E_Image* swapchain_buffer;

E_Material* material;
E_Buffer* vertex_buffer;
E_Buffer* index_buffer;

static f32 vertices[] = {
    -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
     0.5f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f
};

static u32 indices[] = {
    0, 1, 2,
    2, 3, 0
};

void BeginRender()
{
    E_RendererBegin();
    swapchain_buffer = E_GetSwapchainImage();
}

void EndRender()
{
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
    // Initialise Euphorbe
    E_RendererInitSettings settings = { 0 };
    settings.log_found_layers = 0;
    settings.log_renderer_events = 1;
    settings.enable_debug = 1;

    window = E_CreateWindow(1280, 720, "Euphorbe Editor");
    E_RendererInit(window, settings);

    // Renderer assets
    depth_image = E_MakeImage(window->width, window->height, E_ImageFormatD32_Float);

    vertex_buffer = E_CreateVertexBuffer(sizeof(vertices));
    E_SetBufferData(vertex_buffer, vertices, sizeof(vertices));

    index_buffer = E_CreateIndexBuffer(sizeof(indices));
    E_SetBufferData(index_buffer, indices, sizeof(indices));

    material = E_CreateMaterialFromFile("Assets/Materials/RectangleMaterial.toml");

    // Launch the window
    E_WindowSetResizeCallback(window, ResizeCallback);
    E_LaunchWindow(window);

    while (E_IsWindowOpen(window))
    {
        BeginRender();

        // Render loop

        E_ClearValue color_clear = { 0.1f, 0.1f, 0.1f, 1.0f, 0.0f, 0 };
        E_ClearValue depth_clear = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };

        E_ImageAttachment attachments[2] = {
            { swapchain_buffer, E_ImageLayoutColor, color_clear },
            { depth_image, E_ImageLayoutDepth, depth_clear }
        };

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

        E_RendererStartRender(attachments, 2, 1);
        E_BindMaterial(material);
        E_BindBuffer(vertex_buffer);
        E_BindBuffer(index_buffer);
        E_DrawIndexed(0, 6);
        E_RendererEndRender();

        E_ImageTransitionLayout(swapchain_buffer,
            E_ImageAccessColorWrite, 0,
            E_ImageLayoutColor, E_ImageLayoutSwapchainPresent,
            E_ImagePipelineStageColorOutput,
            E_ImagePipelineStageBottom);

        E_BeginGUI();
        E_LogDraw();
        E_EndGUI();

        EndRender();
    }

    E_RendererWait();

    E_FreeBuffer(index_buffer);
    E_FreeBuffer(vertex_buffer);
    E_FreeMaterial(material);
    E_FreeImage(depth_image);

    E_RendererShutdown();
    E_FreeWindow(window);
    return 0;
}