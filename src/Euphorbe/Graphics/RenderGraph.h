#ifndef EUPHORBE_RENDER_GRAPH_H
#define EUPHORBE_RENDER_GRAPH_H

#include <Euphorbe/Core/Common.h>
#include <Euphorbe/Core/CVar.h>
#include <Euphorbe/Graphics/Mesh.h>
#include <Euphorbe/Graphics/Image.h>
#include <Euphorbe/Graphics/Material.h>
#include <Euphorbe/Graphics/Renderer.h>
#include <cglm/cglm.h>

#define EUPHORBE_MAX_DRAWABLE_COUNT 128
#define EUPHORBE_MAX_LIGHT_COUNT 64
#define EUPHORBE_MAX_RENDER_NODE_OUTPUTS 8
#define EUPHORBE_MAX_RENDER_NODE_INPUTS 8
#define EUPHORBE_MAX_RENDER_GRAPH_NODES 16

#define EUPHORBE_DECLARE_NODE_OUTPUT(index) ((~(1u << 31u)) & index)
#define EUPHORBE_DECLARE_NODE_INPUT(index) ((1u << 31u) | index)
#define EUPHORBE_IS_NODE_INPUT(id) (((1u << 31u) & id) > 0)
#define EUPHORBE_GET_NODE_PORT_INDEX(id) (((1u << 31u) - 1u) & id)

typedef struct E_RenderGraphExecuteInfo E_RenderGraphExecuteInfo;
typedef struct E_RenderGraphNode E_RenderGraphNode;
typedef struct E_RenderGraphNodeInput E_RenderGraphNodeInput;
typedef struct E_RenderGraph E_RenderGraph;
typedef struct E_Drawable E_Drawable;
typedef struct E_RenderGraphNodeVector E_RenderGraphNodeVector;

struct E_Drawable
{
	E_Mesh* mesh;
	E_MaterialInstance* geometry_instance; // Contains transform, vertex buffer and meshlet buffer
	E_MaterialInstance* material_instance;
	mat4 transform;
};

struct E_RenderGraphExecuteInfo
{
	E_CVarSystem* cvar_table_ptr;

	E_Drawable drawables[EUPHORBE_MAX_DRAWABLE_COUNT];
	u32 drawable_count;

	E_PointLight point_lights[EUPHORBE_MAX_LIGHT_COUNT];

	mat4 projection;
	mat4 view;
	vec3 camera_position;

	i32 width;
	i32 height;
	i32 frame_index;
};

typedef void (*E_RenderGraphNodeInitFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);
typedef void (*E_RenderGraphNodeCleanFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);
typedef void (*E_RenderGraphNodeExecuteFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);
typedef void (*E_RenderGraphNodeResizeFunc)(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info);

struct E_RenderGraphNodeInput
{
	E_RenderGraphNode* owner;
	u32 index;
};

struct E_RenderGraphNode
{
	void* node_data;
	b32 enabled;
	char* name;

	E_RenderGraphNodeInitFunc init_func;
	E_RenderGraphNodeCleanFunc clean_func;
	E_RenderGraphNodeExecuteFunc execute_func;
	E_RenderGraphNodeResizeFunc resize_func;

	E_Image* outputs[EUPHORBE_MAX_RENDER_NODE_OUTPUTS];
	u32 output_count;

	E_RenderGraphNodeInput inputs[EUPHORBE_MAX_RENDER_NODE_INPUTS];
	u32 input_count;
};

struct E_RenderGraphNodeVector
{
	E_RenderGraphNode** nodes;
	u32 node_count;
};

struct E_RenderGraph
{
	E_RenderGraphNodeVector node_vector;
};

E_RenderGraph* E_CreateRenderGraph();
void E_RenderGraphConnectNodes(E_RenderGraphNode* src_node, u32 src_id, E_RenderGraphNode* dst_node, u32 dst_id);
E_Image* E_GetRenderGraphNodeInputImage(E_RenderGraphNodeInput* input);

void E_BuildRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info, E_RenderGraphNode* last_node);
void E_CleanRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info);
void E_ResizeRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info);
void E_ExecuteRenderGraph(E_RenderGraph* graph, E_RenderGraphExecuteInfo* info);

#endif