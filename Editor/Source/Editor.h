#ifndef EUPHORBE_EDITOR_H
#define EUPHORBE_EDITOR_H

#include <Euphorbe/Euphorbe.h>
#include <cimgui.h>
#include <time.h>

#include "Panels/ViewportPanel.h"
#include "RenderNodes/GeometryNode.h"
#include "EditorCamera.h"

typedef struct EditorPerformance EditorPerformance;
struct EditorPerformance
{
	f64 begin_render;
	f64 execute_render_graph;
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
	E_RenderGraph* graph;
	E_RenderGraphExecuteInfo execute_info;
	E_RenderGraphNode* geometry_node;

	// Textured mesh
	E_ResourceFile* mesh;
	E_ResourceFile* mesh_texture;
	E_MaterialInstance* material_instance;

	// Performance
	EditorPerformance perf;

	// Editor Camera
	EditorCamera camera;
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
void EditorInitialiseTexturedMesh();
void EditorLaunch();
void EditorAssureViewportSize();
void EditorBeginRender();
void EditorEndRender();
void EditorDraw();
void EditorCreateDockspace();
void EditorDestroyDockspace();
void EditorDrawGUI();
void EditorUpdateCameraInput(f32 dt);
f64 EditorBeginProfiling();
f64 EditorEndProfiling(f64 start);

#endif