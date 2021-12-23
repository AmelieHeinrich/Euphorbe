#ifndef EUPHORBE_EDITOR_H
#define EUPHORBE_EDITOR_H

#include <Euphorbe/Euphorbe.h>
#include <cglm/cglm.h>
#include <cimgui.h>
#include <time.h>

#include "Panels/ViewportPanel.h"

typedef struct SceneUniforms SceneUniforms;
struct SceneUniforms
{
	mat4 mvp;
};

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

	// Render state
	b32 first_render;
	E_Image* render_buffer;
	E_Image* depth_buffer;

	// Textured quad
	E_Material* material;
	E_MaterialInstance* material_instance;
	E_Buffer* vertex_buffer;
	E_Buffer* index_buffer;
	E_Buffer* uniform_buffer;
	E_Image* quad_texture;
	SceneUniforms uniforms;

	// Performance
	EditorPerformance perf;
};

extern EditorData editor_state;

void EditorInit();
void EditorCleanup();
void EditorUpdate();
void EditorResize(i32 width, i32 height);

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
f64 EditorBeginProfiling();
f64 EditorEndProfiling(f64 start);

#endif