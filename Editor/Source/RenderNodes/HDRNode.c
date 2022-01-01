#include "HDRNode.h"

#include <Euphorbe/Graphics/Renderer.h>
#include <Euphorbe/Graphics/CommandBuffer.h>

typedef struct HDRNodeData HDRNodeData;
struct HDRNodeData
{
	E_ResourceFile* screen_shader;
	E_MaterialInstance* material_instance;

	E_Buffer* quad_vertex_buffer;
	b32 first_render;

	// params
	f32 exposure;
	b32 enabled;
};

void HDRNodeInit(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	HDRNodeData* data = (HDRNodeData*)node->node_data;

	data->enabled = 1;
	data->exposure = 1.0f;
	
	// Shaders
	E_Image* color_buffer = E_GetRenderGraphNodeInputImage(&node->inputs[0]);
	assert(color_buffer && color_buffer->rhi_handle);

	data->screen_shader = E_LoadResource("Assets/Materials/HDRMaterial.toml", E_ResourceTypeMaterial);
	data->material_instance = E_CreateMaterialInstance(data->screen_shader->as.material, 0);
	E_MaterialInstanceWriteImage(data->material_instance, 0, color_buffer);

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

void HDRNodeClean(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	HDRNodeData* data = (HDRNodeData*)node->node_data;

	E_FreeImage(node->outputs[0]);
	E_FreeBuffer(data->quad_vertex_buffer);
	E_FreeMaterialInstance(data->material_instance);
	E_FreeResource(data->screen_shader);

	free(data);
}

void HDRNodeExecute(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	HDRNodeData* data = (HDRNodeData*)node->node_data;

	E_CommandBuffer* cmd_buf = E_GetSwapchainCommandBuffer();

	vec4 uniform;
	uniform[0] = (f32)data->enabled;
	uniform[1] = (f32)data->exposure;

	E_ImageLayout src_layout = data->first_render ? E_ImageLayoutUndefined : E_ImageLayoutTransferSource;
	data->first_render = 0;

	vec2 render_size = { (f32)info->width, (f32)info->height };

	E_CommandBufferImageTransitionLayout(cmd_buf, node->outputs[0],
		E_ImageAccessShaderRead, E_ImageAccessColorWrite,
		src_layout, E_ImageLayoutColor,
		E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput);

	E_ClearValue color_clear = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0 };

	E_ImageAttachment attachments[1] = {
		{ node->outputs[0], E_ImageLayoutColor, color_clear},
	};

	E_CommandBufferSetViewport(cmd_buf, info->width, info->height);
	E_CommandBufferStartRender(cmd_buf, attachments, 1, render_size, 0);

	E_CommandBufferBindMaterial(cmd_buf, data->screen_shader->as.material);
	E_CommandBufferBindMaterialInstance(cmd_buf, data->material_instance, data->screen_shader->as.material, 0);
	E_CommandBufferPushConstants(cmd_buf, data->screen_shader->as.material, &uniform, sizeof(vec4));
	E_CommandBufferBindBuffer(cmd_buf, data->quad_vertex_buffer);
	E_CommandBufferDraw(cmd_buf, 0, 4);

	E_CommandBufferEndRender(cmd_buf);

	E_CommandBufferImageTransitionLayout(cmd_buf, node->outputs[0],
		E_ImageAccessColorWrite, E_ImageAccessShaderRead,
		E_ImageLayoutColor, E_ImageLayoutTransferSource,
		E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader);
}

void HDRNodeResize(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	HDRNodeData* data = (HDRNodeData*)node->node_data;

	E_FreeImage(node->outputs[0]);
	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);
	data->first_render = 1;

	E_Image* color_buffer = E_GetRenderGraphNodeInputImage(&node->inputs[0]);
	assert(color_buffer && color_buffer->rhi_handle);

	E_MaterialInstanceWriteImage(data->material_instance, 0, color_buffer);
}

E_RenderGraphNode* CreateHDRNode()
{
	E_RenderGraphNode* node = malloc(sizeof(E_RenderGraphNode));

	node->enabled = 1;
	node->init_func = HDRNodeInit;
	node->clean_func = HDRNodeClean;
	node->execute_func = HDRNodeExecute;
	node->resize_func = HDRNodeResize;
	node->node_data = malloc(sizeof(HDRNodeData));
	node->name = "HDRNode";

	node->input_count = 0;
	memset(node->inputs, 0, sizeof(node->inputs));
	
	return node;
}

void HDRNodeDrawGUI(E_RenderGraphNode* node)
{
	HDRNodeData* data = (HDRNodeData*)node->node_data;

	b32 hdr_enabled = igTreeNodeEx_Str("HDR Node", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
	if (hdr_enabled)
	{
		igDragFloat("Exposure", &data->exposure, 0.1f, 0.0f, 0.0f, "%.1f", ImGuiSliderFlags_None);
		igCheckbox("Enable HDR", (bool*)&data->enabled);
		igTreePop();
	}
}
