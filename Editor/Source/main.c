#include <Euphorbe/Euphorbe.h>

#include <cimgui.h>

E_Window* window;
E_Image* render_buffer;
E_Image* depth_image;
E_Image* swapchain_buffer;

typedef struct ColorUniform ColorUniform;
struct ColorUniform
{
    V3 color;
};

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
    E_ImageResize(render_buffer, width, height);
    E_ImageResize(depth_image, width, height);
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
    render_buffer = E_MakeImage(window->width, window->height, E_ImageFormatRGBA8);
    depth_image = E_MakeImage(window->width, window->height, E_ImageFormatD32_Float);

    vertex_buffer = E_CreateVertexBuffer(sizeof(vertices));
    E_SetBufferData(vertex_buffer, vertices, sizeof(vertices));

    index_buffer = E_CreateIndexBuffer(sizeof(indices));
    E_SetBufferData(index_buffer, indices, sizeof(indices));

    V3 color = V3Zero();
    uniform_buffer = E_CreateUniformBuffer(sizeof(ColorUniform));
    E_SetBufferData(uniform_buffer, &color, sizeof(color));

    material = E_CreateMaterialFromFile("Assets/Materials/RectangleMaterial.toml");
    material_instance = E_CreateMaterialInstance(material);

    E_DescriptorInstance descriptor_instance = { 0 };
    descriptor_instance.descriptor = &material->material_create_info->descriptors[0];
    descriptor_instance.buffer.buffer = uniform_buffer;

    E_MaterialInstanceWriteBuffer(material_instance, &descriptor_instance, sizeof(ColorUniform));

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
            { render_buffer, E_ImageLayoutColor, color_clear },
            { depth_image, E_ImageLayoutDepth, depth_clear }
        };

        E_ImageTransitionLayout(render_buffer,
            E_ImageAccessShaderRead, E_ImageAccessColorWrite,
            E_ImageLayoutUndefined, E_ImageLayoutShaderRead,
            E_ImagePipelineStageFragmentShader,
            E_ImagePipelineStageColorOutput);

        E_ImageTransitionLayout(depth_image,
            0, E_ImageAccessDepthWrite,
            E_ImageLayoutUndefined, E_ImageLayoutDepth,
            E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment,
            E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment);

        E_RendererStartRender(attachments, 2, 1);

        E_BindMaterial(material);
        E_BindMaterialInstance(material_instance, material);
        E_SetBufferData(uniform_buffer, &color, sizeof(color));
        E_BindBuffer(vertex_buffer);
        E_BindBuffer(index_buffer);
        E_DrawIndexed(0, 6);

        E_RendererEndRender();

        E_ImageTransitionLayout(render_buffer,
            E_ImageAccessColorWrite, E_ImageAccessShaderRead,
            E_ImageLayoutShaderRead, E_ImageLayoutColor,
            E_ImagePipelineStageColorOutput,
            E_ImagePipelineStageFragmentShader);

        E_BeginGUI();
        
        // Log
        E_LogDraw();

        // Color
        igBegin("Color Panel", NULL, ImGuiWindowFlags_None);
        igColorPicker3("Color Picker", color.data, ImGuiColorEditFlags_DisplayHex | ImGuiColorEditFlags_DisplayRGB);
        igEnd();

        // Viewport
        igBegin("Viewport", NULL, ImGuiWindowFlags_None);
        E_ImageDrawToGUI(render_buffer);
        igEnd();

        E_EndGUI();

        EndRender();
    }

    E_RendererWait();

    E_FreeMaterialInstance(material_instance);
    E_FreeBuffer(uniform_buffer);
    E_FreeBuffer(index_buffer);
    E_FreeBuffer(vertex_buffer);
    E_FreeMaterial(material);
    E_FreeImage(depth_image);
    E_FreeImage(render_buffer);

    E_RendererShutdown();
    E_FreeWindow(window);
    return 0;
}