#pragma once

#include <Euphorbe/Euphorbe.h>
#include <cimgui.h>
#include <time.h>

#include "Panels/ViewportPanel.h"
#include "EditorCamera.h"

#include "RenderNodes/GeometryNode.h"
#include "RenderNodes/FXAANode.h"
#include "RenderNodes/TonemappingNode.h"
#include "RenderNodes/FinalBlitNode.h"

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
	// Config
	E_CVarSystem cvar_sys;

	// Window and misc
	E_Window* window;
	b32 is_viewport_focused;
	b32 is_viewport_hovered;
	f32 last_frame;
	b32 running;
	b32 mesh_shader_enabled;

	// Render state
	E_RenderGraph* graph;
	E_RenderGraphExecuteInfo execute_info;
	
	E_RenderGraphNode* geometry_node;
	E_RenderGraphNode* fxaa_node;
	E_RenderGraphNode* tonemapping_node;
	E_RenderGraphNode* final_blit_node;

	// Textured mesh
	E_Mesh* mesh;

	E_ResourceFile* albedo_texture;
	E_ResourceFile* metallic_roughness_texture;
	E_ResourceFile* normal_texture;
	E_ResourceFile* ao_texture;

	E_Buffer* transform_buffer;
	E_Buffer* material_settings;

	b32 material_buffer[4];
	E_MaterialInstance* material_instance;

	// Performance
	EditorPerformance perf;
	E_PipelineStatistics* stats;

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