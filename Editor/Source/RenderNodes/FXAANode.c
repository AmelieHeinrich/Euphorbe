#include "FXAANode.h"

#include <Euphorbe/Graphics/Renderer.h>
#include <Euphorbe/Graphics/CommandBuffer.h>
#include <Euphorbe/Resource/Resource.h>

typedef struct FXAAPushConstants FXAAPushConstants;
struct FXAAPushConstants
{
	b32 fxaa_enabled;
	b32 show_edges;
	vec2 screen_size;

	f32 fxaa_threshold;
	f32 mul_reduce;
	f32 min_reduce;
	f32 max_span;
};

typedef struct FXAANodeData FXAANodeData;
struct FXAANodeData
{
	E_ResourceFile* fxaa_material;
	E_MaterialInstance* material_instance;
	E_Buffer* quad_vertex_buffer;

	b32 first_render;

	FXAAPushConstants constants;
};

void FXAANodeInit(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	FXAANodeData* data = (FXAANodeData*)node->node_data;

	data->fxaa_material = E_LoadResource("Assets/Materials/FXAAMaterial.toml", E_ResourceTypeMaterial);
	data->material_instance = E_CreateMaterialInstance(data->fxaa_material->as.material, 0);
	
	E_Image* color_input = E_GetRenderGraphNodeInputImage(&node->inputs[0]);
	E_MaterialInstanceWriteImage(data->material_instance, 0, color_input);

	// Screen Quad
	f32 quad_vertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	data->quad_vertex_buffer = E_CreateVertexBuffer(sizeof(quad_vertices));
	E_SetBufferData(data->quad_vertex_buffer, quad_vertices, sizeof(quad_vertices));

	data->first_render = 1;

	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);
	node->output_count = 1;

	// Setup fxaa data
	memset(&data->constants, 0, sizeof(FXAAPushConstants));
	data->constants.fxaa_enabled = E_GetCVar(info->cvar_table_ptr, "enable_fxaa").u.b;
	data->constants.show_edges = 0;

	data->constants.fxaa_threshold = 1.0f / 4.0f;
	data->constants.max_span = 8.0f;
	data->constants.mul_reduce = 1.0f / data->constants.max_span;
	data->constants.min_reduce = 1.0f / 128.0f;
}

void FXAANodeClean(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	FXAANodeData* data = (FXAANodeData*)node->node_data;
	
	E_FreeImage(node->outputs[0]);
	E_FreeBuffer(data->quad_vertex_buffer);
	E_FreeMaterialInstance(data->material_instance);
	E_FreeResource(data->fxaa_material);

	free(data);
}

void FXAANodeExecute(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	FXAANodeData* data = (FXAANodeData*)node->node_data;

	E_CommandBuffer* cmd_buf = E_GetSwapchainCommandBuffer();

	data->constants.screen_size[0] = (f32)info->width;
	data->constants.screen_size[1] = (f32)info->height;

	E_ImageLayout src_layout = data->first_render ? E_ImageLayoutUndefined : E_ImageLayoutShaderRead;
	data->first_render = 0;

	vec2 render_size = { (f32)info->width, (f32)info->height };

	E_CommandBufferImageTransitionLayout(cmd_buf, node->outputs[0],
		E_ImageAccessShaderRead, E_ImageAccessColorWrite,
		src_layout, E_ImageLayoutColor,
		E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput, 0);

	E_ClearValue color_clear = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0 };

	E_ImageAttachment attachments[1] = {
		{ node->outputs[0], E_ImageLayoutColor, color_clear},
	};

	E_CommandBufferSetViewport(cmd_buf, info->width, info->height);
	E_CommandBufferStartRender(cmd_buf, attachments, 1, render_size, 0);

	E_CommandBufferBindMaterial(cmd_buf, data->fxaa_material->as.material);
	E_CommandBufferBindMaterialInstance(cmd_buf, data->material_instance, data->fxaa_material->as.material, 0);
	E_CommandBufferPushConstants(cmd_buf, data->fxaa_material->as.material, &data->constants, sizeof(FXAAPushConstants));
	E_CommandBufferBindBuffer(cmd_buf, data->quad_vertex_buffer);
	E_CommandBufferDraw(cmd_buf, 0, 4);

	E_CommandBufferEndRender(cmd_buf);

	E_CommandBufferImageTransitionLayout(cmd_buf, node->outputs[0],
		E_ImageAccessColorWrite, E_ImageAccessShaderRead,
		E_ImageLayoutColor, E_ImageLayoutShaderRead,
		E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader, 0);
}

void FXAANodeResize(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	FXAANodeData* data = (FXAANodeData*)node->node_data;

	E_FreeImage(node->outputs[0]);
	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);

	E_Image* color_input = E_GetRenderGraphNodeInputImage(&node->inputs[0]);
	E_MaterialInstanceWriteImage(data->material_instance, 0, color_input);

	data->first_render = 1;
}

E_RenderGraphNode* CreateFXAANode()
{
	E_RenderGraphNode* node = malloc(sizeof(E_RenderGraphNode));

	node->enabled = 1;
	node->init_func = FXAANodeInit;
	node->clean_func = FXAANodeClean;
	node->resize_func = FXAANodeResize;
	node->execute_func = FXAANodeExecute;
	node->name = "FXAANode";
	node->node_data = malloc(sizeof(FXAANodeData));
	memset(node->node_data, 0, sizeof(node->node_data));

	node->input_count = 0;
	memset(node->inputs, 0, sizeof(node->inputs));

	return node;
}

void FXAANodeDrawGUI(E_RenderGraphNode* node)
{
	FXAANodeData* data = (FXAANodeData*)node->node_data;

	b32 fxaa_node = igTreeNodeEx_Str("FXAA Node", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
	if (fxaa_node)
	{
		igCheckbox("Enable FXAA", (bool*)&data->constants.fxaa_enabled);
		igCheckbox("Show Edges", (bool*)&data->constants.show_edges);
		
		igDragFloat("Luma Threshold", &data->constants.fxaa_threshold, 0.01f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None);
		igDragFloat("Multiply Reduce", &data->constants.mul_reduce, 0.01f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None);
		igDragFloat("Minimum Reduce", &data->constants.min_reduce, 0.01f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None);
		igDragFloat("Maximum Span", &data->constants.max_span, 0.01f, 0.0f, 0.0f, "%.2f", ImGuiSliderFlags_None);
		igTreePop();
	}
}
