#ifndef EDITOR_VIEWPORT_PANEL_H
#define EDITOR_VIEWPORT_PANEL_H

#include <cimgui.h>
#include <cglm/cglm.h>
#include <Euphorbe/Graphics/Image.h>

typedef struct ViewportPanel ViewportPanel;
struct ViewportPanel
{
	vec2 viewport_size;
};

extern ViewportPanel viewport_panel;

void InitViewportPanel(int width, int height);
void DrawViewportPanel(E_Image* buffer, b32* is_viewport_focused);

#endif