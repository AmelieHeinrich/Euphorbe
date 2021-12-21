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

    igBegin("Viewport", NULL, ImGuiWindowFlags_None);

    ImVec2 panel_content_size = { 0.0f, 0.0f };
    igGetContentRegionAvail(&panel_content_size);

    viewport_panel.viewport_size.x = panel_content_size.x;
    viewport_panel.viewport_size.y = panel_content_size.y;

    E_ImageDrawToGUI(buffer, (i32)viewport_panel.viewport_size.x, (i32)viewport_panel.viewport_size.y);
    igEnd();

    igPopStyleVar(0);
}
