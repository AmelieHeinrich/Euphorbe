#include "ViewportPanel.h"

ViewportPanel viewport_panel;

void InitViewportPanel(int width, int height)
{
    viewport_panel.viewport_size[0] = width;
    viewport_panel.viewport_size[1] = height;
}

void DrawViewportPanel(E_Image* buffer, b32* is_viewport_focused)
{
    igPushStyleVar_Float(ImGuiStyleVar_WindowPadding, 0.0f);

    igBegin("Viewport", NULL, ImGuiWindowFlags_None);

    *is_viewport_focused = igIsWindowFocused(ImGuiFocusedFlags_None) && igIsWindowHovered(ImGuiHoveredFlags_None);
    ImVec2 panel_content_size = { 0.0f, 0.0f };
    igGetContentRegionAvail(&panel_content_size);

    viewport_panel.viewport_size[0] = panel_content_size.x;
    viewport_panel.viewport_size[1] = panel_content_size.y;

    E_ImageDrawToGUI(buffer, (i32)viewport_panel.viewport_size[0], (i32)viewport_panel.viewport_size[1]);
    igEnd();

    igPopStyleVar(0);
}
