#include "ViewportPanel.h"

ViewportPanel viewport_panel;

void InitViewportPanel(int width, int height)
{
    viewport_panel.viewport_size.x = width;
    viewport_panel.viewport_size.y = height;
}

void DrawViewportPanel(E_Image* buffer)
{
    igPushStyleVar_Float(ImGuiStyleVar_WindowPadding, 0.0f);

    ImVec2 panel_content_size = {buffer->width, buffer->height};
    
    viewport_panel.viewport_size.x = panel_content_size.x;
    viewport_panel.viewport_size.y = panel_content_size.y;

    igBegin("Viewport", NULL, ImGuiWindowFlags_None);
    E_ImageDrawToGUI(buffer, viewport_panel.viewport_size.x, viewport_panel.viewport_size.y);
    igEnd();

    igPopStyleVar(0);
}
