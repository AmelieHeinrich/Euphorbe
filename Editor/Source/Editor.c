#include "Editor.h"

EditorData editor_state;

void EditorInit()
{
    EditorInitialiseWindow();
    EditorInitialiseRenderState();
    EditorInitialiseTexturedQuad();
    EditorLaunch();
}

void EditorCleanup()
{
    E_RendererWait();

    E_FreeMaterialInstance(editor_state.material_instance);
    E_FreeImage(editor_state.quad_texture);
    E_FreeBuffer(editor_state.uniform_buffer);
    E_FreeBuffer(editor_state.index_buffer);
    E_FreeBuffer(editor_state.vertex_buffer);
    E_FreeMaterial(editor_state.material);
    E_FreeImage(editor_state.depth_buffer);
    E_FreeImage(editor_state.render_buffer);

    E_RendererShutdown();
    E_FreeWindow(editor_state.window);
}

void EditorUpdate()
{
    while (E_IsWindowOpen(editor_state.window))
    {
        EditorAssureViewportSize();

        EditorBeginRender();

        EditorDrawTexturedQuad();
        EditorDrawGUI();

        EditorEndRender();
    }
}

void EditorResize(i32 width, i32 height)
{
    E_RendererResize(width, height);

    E_ImageResize(editor_state.render_buffer, viewport_panel.viewport_size[0], viewport_panel.viewport_size[1]);
    E_ImageResize(editor_state.depth_buffer, viewport_panel.viewport_size[0], viewport_panel.viewport_size[1]);
    editor_state.first_render = 1;
}

//

void EditorInitialiseWindow()
{
    // Initialise Euphorbe
    E_RendererInitSettings settings = { 0 };
    settings.log_found_layers = 0;
    settings.log_renderer_events = 1;
    settings.enable_debug = 1;
    settings.gui_should_clear = 1;

    editor_state.window = E_CreateWindow(1280, 720, "Euphorbe Editor");
    E_RendererInit(editor_state.window, settings);
}

void EditorInitialiseRenderState()
{
    editor_state.render_buffer = E_MakeImage(editor_state.window->width, editor_state.window->height, E_ImageFormatRGBA8);
    editor_state.depth_buffer = E_MakeImage(editor_state.window->width, editor_state.window->height, E_ImageFormatD32_Float);
}

void EditorInitialiseTexturedQuad()
{
    f32 vertices[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f,  0.5f, 0.0f, 0.0f, 1.0f
    };

    u32 indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    editor_state.vertex_buffer = E_CreateVertexBuffer(sizeof(vertices));
    E_SetBufferData(editor_state.vertex_buffer, vertices, sizeof(vertices));

    editor_state.index_buffer = E_CreateIndexBuffer(sizeof(indices));
    E_SetBufferData(editor_state.index_buffer, indices, sizeof(indices));

    editor_state.quad_texture = E_MakeImageFromFile("Assets/Textures/awesomeface.png");

    glm_mat4_identity(&editor_state.uniforms.mvp);
    editor_state.uniform_buffer = E_CreateUniformBuffer(sizeof(editor_state.uniforms));
    E_SetBufferData(editor_state.uniform_buffer, &editor_state.uniforms, sizeof(editor_state.uniforms));

    editor_state.material = E_CreateMaterialFromFile("Assets/Materials/RectangleMaterial.toml");
    editor_state.material_instance = E_CreateMaterialInstance(editor_state.material);

    E_DescriptorInstance buffer_instance = { 0 };
    buffer_instance.descriptor = &editor_state.material->material_create_info->descriptors[0];
    buffer_instance.buffer.buffer = editor_state.uniform_buffer;

    E_DescriptorInstance texture_instance = { 0 };
    texture_instance.descriptor = &editor_state.material->material_create_info->descriptors[1];
    texture_instance.image.image = editor_state.quad_texture;
    texture_instance.image.layout = E_ImageLayoutShaderRead;

    E_MaterialInstanceWriteBuffer(editor_state.material_instance, &buffer_instance, sizeof(mat4));
    E_MaterialInstanceWriteImage(editor_state.material_instance, &texture_instance);
}

void EditorLaunch()
{
    InitViewportPanel(editor_state.window->width, editor_state.window->height);
    E_WindowSetResizeCallback(editor_state.window, EditorResize);
    E_LaunchWindow(editor_state.window);
}

void EditorAssureViewportSize()
{
    if (viewport_panel.viewport_size[0] != (f32)editor_state.render_buffer->width || viewport_panel.viewport_size[1] != (f32)editor_state.render_buffer->height && viewport_panel.viewport_size[0] > 0 && viewport_panel.viewport_size[1] > 0)
    {
        E_RendererWait();
        E_ImageResize(editor_state.render_buffer, viewport_panel.viewport_size[0], viewport_panel.viewport_size[1]);
        E_ImageResize(editor_state.depth_buffer, viewport_panel.viewport_size[0], viewport_panel.viewport_size[1]);
        editor_state.first_render = 1;
    }
}

void EditorBeginRender()
{
    E_RendererBegin();

    glm_mat4_identity(editor_state.uniforms.mvp);

    E_ClearValue color_clear = { 0.1f, 0.2f, 0.3f, 1.0f, 0.0f, 0 };
    E_ClearValue depth_clear = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };

    E_ImageAttachment attachments[2] = {
        { editor_state.render_buffer, E_ImageLayoutColor, color_clear },
        { editor_state.depth_buffer,   E_ImageLayoutDepth, depth_clear }
    };

    E_ImageLayout src_render_buffer_image_layout = editor_state.first_render ? E_ImageLayoutUndefined : E_ImageLayoutColor;
    editor_state.first_render = 0;

    vec2 render_size = { viewport_panel.viewport_size[0], viewport_panel.viewport_size[1] };

    E_RendererStartRender(attachments, 2, render_size, 1);

    E_ImageTransitionLayout(editor_state.render_buffer,
        E_ImageAccessShaderRead, E_ImageAccessColorWrite,
        src_render_buffer_image_layout, E_ImageLayoutShaderRead,
        E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput);

    E_ImageTransitionLayout(editor_state.depth_buffer,
        0, E_ImageAccessDepthWrite,
        E_ImageLayoutUndefined, E_ImageLayoutDepth,
        E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment,
        E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment);
}

void EditorEndRender()
{
    E_ImageTransitionLayout(editor_state.render_buffer,
        E_ImageAccessColorWrite, E_ImageAccessShaderRead,
        E_ImageLayoutShaderRead, E_ImageLayoutColor,
        E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader);

    E_RendererEnd();
    E_WindowUpdate(editor_state.window);
}

void EditorDrawTexturedQuad()
{
    E_BindMaterial(editor_state.material);
    E_BindMaterialInstance(editor_state.material_instance, editor_state.material);
    E_SetBufferData(editor_state.uniform_buffer, &editor_state.uniforms, sizeof(editor_state.uniforms));
    E_BindBuffer(editor_state.vertex_buffer);
    E_BindBuffer(editor_state.index_buffer);
    E_DrawIndexed(0, 6);

    E_RendererEndRender();
}

void EditorDrawGUI()
{
    EditorCreateDockspace();

    E_LogDraw();
    DrawViewportPanel(editor_state.render_buffer);

    igBegin("Material Viewer", NULL, ImGuiWindowFlags_AlwaysAutoResize);
    igText("Albedo Map:");
    E_ImageDrawToGUI(editor_state.quad_texture, editor_state.quad_texture->width / 2, editor_state.quad_texture->height / 2);
    igEnd();

    EditorDestroyDockspace();
}

void EditorCreateDockspace()
{
    E_BeginGUI();

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

void EditorDestroyDockspace()
{
    igEnd();

    E_EndGUI();
}
