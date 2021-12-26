#ifndef EUPHORBE_EDITOR_H
#define EUPHORBE_EDITOR_H

#include <Euphorbe/Euphorbe.h>
#include <cimgui.h>
#include <time.h>

#include "Panels/ViewportPanel.h"
#include "EditorCamera.h"

typedef struct EditorPerformance EditorPerformance;
struct EditorPerformance
{
	f64 begin_render;
	f64 draw_quad;
	f64 draw_gui;
	f64 end_render;
};

typedef struct EditorData EditorData;
struct EditorData
{
	E_Window* window;
	b32 is_viewport_focused;
	f32 last_frame;
	b32 running;

	// Render state
	b32 first_render;
	E_Image* render_buffer;
	E_Image* depth_buffer;

	// Textured quad
	E_ResourceFile* material;
	E_MaterialInstance* material_instance;
	E_Buffer* vertex_buffer;
	E_Buffer* index_buffer;
	E_ResourceFile* quad_texture;

	// Performance
	EditorPerformance perf;

	// Editor Camera
	EditorCamera camera;

	// Scene Settings
	vec4 clear_color;
};

extern EditorData editor_state;

void EditorInit();
void EditorCleanup();
void EditorUpdate();
void EditorResize(i32 width, i32 height);
void EditorScroll(f32 scroll);

//
void EditorInitialiseWindow();
void EditorInitialiseRenderState();
void EditorInitialiseTexturedQuad();
void EditorLaunch();
void EditorAssureViewportSize();
void EditorBeginRender();
void EditorEndRender();
void EditorDrawTexturedQuad();
void EditorCreateDockspace();
void EditorDestroyDockspace();
void EditorDrawGUI();
void EditorUpdateCameraInput(f32 dt);
f64 EditorBeginProfiling();
f64 EditorEndProfiling(f64 start);

#endif