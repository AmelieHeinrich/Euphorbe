#ifndef EUPHORBE_EDITOR_H
#define EUPHORBE_EDITOR_H

#include <Euphorbe/Euphorbe.h>
#include <cglm/cglm.h>
#include <cimgui.h>

#include "Panels/ViewportPanel.h"

typedef struct SceneUniforms SceneUniforms;
struct SceneUniforms
{
	mat4 mvp;
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

#endif