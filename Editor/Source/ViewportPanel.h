#ifndef EDITOR_VIEWPORT_PANEL_H
#define EDITOR_VIEWPORT_PANEL_H

#include <cimgui.h>
#include <Euphorbe/Math/Math.h>
#include <Euphorbe/Graphics/Image.h>

typedef struct ViewportPanel ViewportPanel;
struct ViewportPanel
{
	V2 viewport_size;
};

extern ViewportPanel viewport_panel;

void InitViewportPanel(int width, int height);
void DrawViewportPanel(E_Image* buffer);

#endif