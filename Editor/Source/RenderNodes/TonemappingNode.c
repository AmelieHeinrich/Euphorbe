#include "TonemappingNode.h"

#include <Euphorbe/Graphics/Renderer.h>
#include <Euphorbe/Graphics/CommandBuffer.h>
#include <Euphorbe/Resource/Resource.h>

typedef struct TonemappingConstants TonemappingConstants;
struct TonemappingConstants
{
	b32 gamma_correct;
	i32 mode;

	vec2 pad;
};

typedef struct TonemappingNodeData TonemappingNodeData;
struct TonemappingNodeData
{
	E_ResourceFile* screen_shader;
	E_MaterialInstance* material_instance;

	E_Buffer* quad_vertex_buffer;
	b32 first_render;

	// params
	TonemappingConstants constants;
};

void TonemappingNodeInit(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	TonemappingNodeData* data = (TonemappingNodeData*)node->node_data;

	data->constants.gamma_correct = E_GetCVar(info->cvar_table_ptr, "gamma_correction").u.b;
	data->constants.mode = (i32)E_GetCVar(info->cvar_table_ptr, "tonemapping_curve").u.i;
	
	// Shaders
	E_Image* color_buffer = E_GetRenderGraphNodeInputImage(&node->inputs[0]);
	assert(color_buffer && color_buffer->rhi_handle);

	data->screen_shader = E_LoadResource("Assets/Materials/TonemappingMaterial.toml", E_ResourceTypeMaterial);
	data->material_instance = E_CreateMaterialInstance(data->screen_shader->as.material, 0);
	E_MaterialInstanceWriteSampler(data->material_instance, 0, E_LinearSampler);
	E_MaterialInstanceWriteSampledImage(data->material_instance, 1, color_buffer);

	// Screen Quad
	f32 quad_vertices[] = {
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};

	data->quad_vertex_buffer = E_CreateVertexBuffer(sizeof(quad_vertices));
	E_SetBufferData(data->quad_vertex_buffer, quad_vertices, sizeof(quad_vertices));

	// Actual output hdr buffer
	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);
	node->output_count++;
	data->first_render = 1;
}

void TonemappingNodeClean(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	TonemappingNodeData* data = (TonemappingNodeData*)node->node_data;

	E_FreeImage(node->outputs[0]);
	E_FreeBuffer(data->quad_vertex_buffer);
	E_FreeMaterialInstance(data->material_instance);
	E_FreeResource(data->screen_shader);

	free(data);
}

void TonemappingNodeExecute(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	TonemappingNodeData* data = (TonemappingNodeData*)node->node_data;

	E_CommandBuffer* cmd_buf = E_GetSwapchainCommandBuffer();

	E_ImageLayout src_layout = data->first_render ? E_ImageLayoutUndefined : E_ImageLayoutTransferSource;
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

	E_CommandBufferBindMaterial(cmd_buf, data->screen_shader->as.material);
	E_CommandBufferBindMaterialInstance(cmd_buf, data->material_instance, data->screen_shader->as.material, 0);
	E_CommandBufferPushConstants(cmd_buf, data->screen_shader->as.material, &data->constants, sizeof(TonemappingConstants));
	E_CommandBufferBindBuffer(cmd_buf, data->quad_vertex_buffer);
	E_CommandBufferDraw(cmd_buf, 0, 4);

	E_CommandBufferEndRender(cmd_buf);

	E_CommandBufferImageTransitionLayout(cmd_buf, node->outputs[0],
		E_ImageAccessColorWrite, E_ImageAccessShaderRead,
		E_ImageLayoutColor, E_ImageLayoutTransferSource,
		E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader, 0);
}

void TonemappingNodeResize(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	TonemappingNodeData* data = (TonemappingNodeData*)node->node_data;

	E_FreeImage(node->outputs[0]);
	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);
	data->first_render = 1;

	E_Image* color_buffer = E_GetRenderGraphNodeInputImage(&node->inputs[0]);
	assert(color_buffer && color_buffer->rhi_handle);

	E_MaterialInstanceWriteSampledImage(data->material_instance, 1, color_buffer);
}

E_RenderGraphNode* CreateTonemappingNode()
{
	E_RenderGraphNode* node = malloc(sizeof(E_RenderGraphNode));

	node->enabled = 1;
	node->init_func = TonemappingNodeInit;
	node->clean_func = TonemappingNodeClean;
	node->execute_func = TonemappingNodeExecute;
	node->resize_func = TonemappingNodeResize;
	node->node_data = malloc(sizeof(TonemappingNodeData));
	memset(node->node_data, 0, sizeof(node->node_data));
	node->name = "TonemappingNode";

	node->input_count = 0;
	memset(node->inputs, 0, sizeof(node->inputs));
	
	return node;
}

void TonemappingNodeDrawGUI(E_RenderGraphNode* node)
{
	TonemappingNodeData* data = (TonemappingNodeData*)node->node_data;

	static const char* modes[] = { "ACES", "Reinhard", "Luma Reinhard", "White Preserving Luma Reinhard", "Rom Bin Da House", "Filmic", "Uncharted 2", "None" };

	b32 tone_enabled = igTreeNodeEx_Str("Tonemapping Node", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
	if (tone_enabled)
	{
		igCheckbox("Enable Gamma Correction", &data->constants.gamma_correct);
		igCombo_Str_arr("Tonemapping Algorithms", &data->constants.mode, modes, 8, 0);
		igTreePop();
	}
}
