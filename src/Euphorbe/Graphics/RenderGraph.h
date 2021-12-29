#ifndef EUPHORBE_RENDER_GRAPH_H
#define EUPHORBE_RENDER_GRAPH_H

#include <Euphorbe/Core/Common.h>
#include <Euphorbe/Graphics/Mesh.h>
#include <Euphorbe/Graphics/Image.h>
#include <Euphorbe/Graphics/Material.h>
#include <cglm/cglm.h>

#define EUPHORBE_MAX_DRAWABLE_COUNT 128
#define EUPHORBE_MAX_RENDER_NODE_OUTPUTS 8
#define EUPHORBE_MAX_RENDER_GRAPH_NODES 16

typedef struct E_RenderGraphExecuteInfo E_RenderGraphExecuteInfo;
typedef struct E_RenderGraphNode E_RenderGraphNode;
typedef struct E_RenderGraph E_RenderGraph;
typedef struct E_Drawable E_Drawable;

struct E_Drawable
{
	E_Mesh* mesh;
	E_MaterialInstance* material_instance;
};

struct E_RenderGraphExecuteInfo
{
	E_Drawable drawables[EUPHORBE_MAX_DRAWABLE_COUNT];
	i32 drawable_count;

	mat4 camera;
	i32 width;
	i32 height;
	i32 frame_index;
};

typedef void (*E_RenderGraphNodeInitFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);
typedef void (*E_RenderGraphNodeCleanFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);
typedef void (*E_RenderGraphNodeExecuteFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);
typedef void (*E_RenderGraphNodeResizeFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);

struct E_RenderGraphNode
{
	void* node_data;
	b32 enabled;
	char* name;
	E_Image* output;

	E_RenderGraphNodeInitFunc init_func;
	E_RenderGraphNodeCleanFunc clean_func;
	E_RenderGraphNodeExecuteFunc execute_func;
	E_RenderGraphNodeResizeFunc resize_func;
};

struct E_RenderGraph
{
	E_RenderGraphNode* nodes[EUPHORBE_MAX_RENDER_GRAPH_NODES];
	i32 node_count;
};

E_RenderGraph* E_CreateRenderGraph();
void E_CleanRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info);
void E_ResizeRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info);
void E_ExecuteRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info);

void E_AddNodeToRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info, E_RenderGraphNode* node);
E_RenderGraphNode* E_GetRenderGraphNode(E_RenderGraph* graph, char* name);

#endif