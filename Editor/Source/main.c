#include <Euphorbe/Euphorbe.h>

#include <cimgui.h>

#include "ViewportPanel.h"

i32 first_render = FRAMES_IN_FLIGHT;
E_Window* window;
E_Image* render_buffers[FRAMES_IN_FLIGHT];
E_Image* depth_image;
V3 color;

E_Material* material;
E_MaterialInstance* material_instance;
E_Buffer* vertex_buffer;
E_Buffer* index_buffer;
E_Buffer* uniform_buffer;

static f32 vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.5f,  0.5f, 0.0f,
    -0.5f,  0.5f, 0.0f
};

static u32 indices[] = {
    0, 1, 2,
    2, 3, 0
};

void BeginRender()
{
    E_RendererBegin();
}

void EndRender(E_Image* render_buffer)
{
    E_ImageTransitionLayout(render_buffer,
        E_ImageAccessColorWrite, E_ImageAccessShaderRead,
        E_ImageLayoutShaderRead, E_ImageLayoutColor,
        E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader);

    E_RendererEnd();
    E_WindowUpdate(window);
}

void ResizeCallback(i32 width, i32 height)
{
    E_RendererResize(width, height);

    for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        E_ImageResize(render_buffers[i], viewport_panel.viewport_size.x, viewport_panel.viewport_size.y);
    E_ImageResize(depth_image, viewport_panel.viewport_size.x, viewport_panel.viewport_size.y);
    first_render = 3;
}

void DrawQuad(E_Image* render_buffer)
{
    E_ClearValue color_clear = { 1.0f, 0.1f, 0.1f, 1.0f, 0.0f, 0 };
    E_ClearValue depth_clear = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };

    E_ImageAttachment attachments[2] = {
        { render_buffer, E_ImageLayoutColor, color_clear },
        { depth_image,   E_ImageLayoutDepth, depth_clear }
    };

    E_ImageLayout src_render_buffer_image_layout = first_render ? E_ImageLayoutUndefined : E_ImageLayoutColor;
    first_render -= 1;

    V2 render_size = { viewport_panel.viewport_size.x, viewport_panel.viewport_size.y };

    E_RendererStartRender(attachments, 2, render_size, 1);

    E_ImageTransitionLayout(render_buffer,
        E_ImageAccessShaderRead, E_ImageAccessColorWrite,
        src_render_buffer_image_layout, E_ImageLayoutShaderRead,
        E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput);

    E_ImageTransitionLayout(depth_image,
        0, E_ImageAccessDepthWrite,
        E_ImageLayoutUndefined, E_ImageLayoutDepth,
        E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment,
        E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment);

    E_BindMaterial(material);
    E_BindMaterialInstance(material_instance, material);
    E_SetBufferData(uniform_buffer, &color, sizeof(color));
    E_BindBuffer(vertex_buffer);
    E_BindBuffer(index_buffer);
    E_DrawIndexed(0, 6);

    E_RendererEndRender();
}

void BeginDockspace()
{
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGuiViewport* viewport = igGetMainViewport();
    igSetNextWindowPos(viewport->Pos, ImGuiCond_None, (ImVec2) { 0.0f, 0.0f });
    igSetNextWindowSize(viewport->Size, ImGuiCond_None);
    igSetNextWindowViewport(viewport->ID);
    igPushStyleVar_Float(ImGuiStyleVar_WindowRounding, 0.0f);
    igPushStyleVar_Float(ImGuiStyleVar_WindowBorderSize, 0.0f);
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2) { 0.0f, 0.0f });
    igBegin("Dockspace", NULL, window_flags);
    igPopStyleVar(3);

    igDockSpace(igGetID_Str("MyDockSpace"), (ImVec2) { 0.0f, 0.0f }, ImGuiDockNodeFlags_None, NULL);
}

void EndDockspace()
{
    igEnd();
}

int main()
{
    // Initialise Euphorbe
    E_RendererInitSettings settings = { 0 };
    settings.log_found_layers = 0;
    settings.log_renderer_events = 1;
    settings.enable_debug = 1;
    settings.gui_should_clear = 1;

    window = E_CreateWindow(1280, 720, "Euphorbe Editor");
    E_RendererInit(window, settings);

    // Renderer assets
    for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        render_buffers[i] = E_MakeImage(window->width, window->height, E_ImageFormatRGBA8);
    depth_image = E_MakeImage(window->width, window->height, E_ImageFormatD32_Float);

    vertex_buffer = E_CreateVertexBuffer(sizeof(vertices));
    E_SetBufferData(vertex_buffer, vertices, sizeof(vertices));

    index_buffer = E_CreateIndexBuffer(sizeof(indices));
    E_SetBufferData(index_buffer, indices, sizeof(indices));

    color = V3Zero();
    uniform_buffer = E_CreateUniformBuffer(sizeof(V3));
    E_SetBufferData(uniform_buffer, &color, sizeof(color));

    material = E_CreateMaterialFromFile("Assets/Materials/RectangleMaterial.toml");
    material_instance = E_CreateMaterialInstance(material);

    E_DescriptorInstance descriptor_instance = { 0 };
    descriptor_instance.descriptor = &material->material_create_info->descriptors[0];
    descriptor_instance.buffer.buffer = uniform_buffer;

    E_MaterialInstanceWriteBuffer(material_instance, &descriptor_instance, sizeof(V3));

    // Launch the window
    InitViewportPanel(window->width, window->height);
    E_WindowSetResizeCallback(window, ResizeCallback);
    E_LaunchWindow(window);

    while (E_IsWindowOpen(window))
    {
        // Update viewport
        if (viewport_panel.viewport_size.x != (f32)render_buffers[0]->width || viewport_panel.viewport_size.y != (f32)render_buffers[0]->height && viewport_panel.viewport_size.x > 0 && viewport_panel.viewport_size.y > 0)
        {
            E_RendererWait();
            for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
                E_ImageResize(render_buffers[i], viewport_panel.viewport_size.x, viewport_panel.viewport_size.y);
            E_ImageResize(depth_image, viewport_panel.viewport_size.x, viewport_panel.viewport_size.y);
            first_render = 3;
        }

        BeginRender();

        u32 image_index = E_GetSwapchainImageIndex();
        E_Image* render_buffer = render_buffers[image_index];

        // DRAW QUAD //
        DrawQuad(render_buffer);
        // END DRAW QUAD //

        // GUI //
        E_BeginGUI();

        BeginDockspace();
        E_LogDraw();
        DrawViewportPanel(render_buffer);
        EndDockspace();

        E_EndGUI();
        // END GUI //

        EndRender(render_buffer);
    }

    E_RendererWait();

    E_FreeMaterialInstance(material_instance);
    E_FreeBuffer(uniform_buffer);
    E_FreeBuffer(index_buffer);
    E_FreeBuffer(vertex_buffer);
    E_FreeMaterial(material);
    E_FreeImage(depth_image);
    for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        E_FreeImage(render_buffers[i]);

    E_RendererShutdown();
    E_FreeWindow(window);
    return 0;
}