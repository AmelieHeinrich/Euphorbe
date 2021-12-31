#include "GeometryNode.h"

#include <Euphorbe/Graphics/Renderer.h>

typedef struct GeometryUniforms GeometryUniforms;
struct GeometryUniforms
{
	mat4 projection;
	mat4 view;
	mat4 prev_view;
	vec3 camera_position;
	f32 padding;
};

typedef struct GeometryData GeometryData;
struct GeometryData
{
	b32 first_render;
	b32 skybox_enabled;

	E_Buffer* light_buffer;
	E_MaterialInstance* light_material_instance;

	E_ResourceFile* geometry_material;
	E_ResourceFile* skybox_material;

	E_ResourceFile* skybox_mesh;
	E_MaterialInstance* skybox_instance;
	E_Image* hdr_image;

	GeometryUniforms uniforms;
};

f32 Halton(i32 index, i32 base)
{
	i32 i = index % 1024;
	i32 b = base;
	f32 f = 1.0f;
	f32 r = 0.0f;

	while (i > 0)
	{
		f = f / (f32)b;
		r = r + f * (i % b);
		i = i / b;
	}

	return r;
}

void GeometryNodeInit(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);
	node->outputs[1] = E_MakeImage(info->width, info->height, E_ImageFormatRG16, E_ImageUsageRenderGraphNodeOutput);
	node->outputs[2] = E_MakeImage(info->width, info->height, E_ImageFormatD32_Float, E_ImageUsageDepthStencilAttachment | E_ImageUsageSampled);
	node->output_count = 3;

	data->first_render = 1;
	data->geometry_material = E_LoadResource("Assets/Materials/GeometryMaterial.toml", E_ResourceTypeMaterial);

	data->skybox_material = E_LoadResource("Assets/Materials/SkyboxMaterial.toml", E_ResourceTypeMaterial);
	data->skybox_mesh = E_LoadResource("Assets/Models/Cube.gltf", E_ResourceTypeMesh);
	data->skybox_instance = E_CreateMaterialInstance(data->skybox_material->as.material, 0);
	data->hdr_image = E_MakeHDRImageFromFile("Assets/EnvMaps/SnowyField/4k.hdr");
	data->skybox_enabled = 1;

	E_MaterialInstanceWriteImage(data->skybox_instance, 0, data->hdr_image);

	data->light_buffer = E_CreateUniformBuffer(sizeof(info->point_lights));
	data->light_material_instance = E_CreateMaterialInstance(data->geometry_material->as.material, 1);
	E_MaterialInstanceWriteBuffer(data->light_material_instance, 0, data->light_buffer, sizeof(info->point_lights));
	E_MaterialInstanceWriteImage(data->light_material_instance, 1, data->hdr_image);
}

void GeometryNodeClean(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_FreeBuffer(data->light_buffer);
	E_FreeMaterialInstance(data->light_material_instance);

	E_FreeResource(data->skybox_mesh);
	E_FreeImage(data->hdr_image);
	E_FreeMaterialInstance(data->skybox_instance);
	E_FreeResource(data->skybox_material);

	E_FreeImage(node->outputs[2]);
	E_FreeImage(node->outputs[1]);
	E_FreeImage(node->outputs[0]);
	E_FreeResource(data->geometry_material);
	free(data);
}

void GeometryNodeExecute(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_ClearValue color_clear = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0 };
	E_ClearValue depth_clear = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };

	E_ImageAttachment attachments[3] = {
		{ node->outputs[0], E_ImageLayoutColor, color_clear},
		{ node->outputs[1], E_ImageLayoutColor, color_clear},
		{ node->outputs[2], E_ImageLayoutDepth, depth_clear}
	};

	E_ImageLayout src_render_buffer_image_layout = data->first_render ? E_ImageLayoutUndefined : E_ImageLayoutShaderRead;
	data->first_render = 0;

	vec2 render_size = { info->width, info->height };

	E_ImageTransitionLayout(node->outputs[0],
		E_ImageAccessShaderRead, E_ImageAccessColorWrite,
		src_render_buffer_image_layout, E_ImageLayoutColor,
		E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput);

	E_ImageTransitionLayout(node->outputs[1],
		E_ImageAccessShaderRead, E_ImageAccessColorWrite,
		src_render_buffer_image_layout, E_ImageLayoutColor,
		E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput);

	E_ImageTransitionLayout(node->outputs[2],
		0, E_ImageAccessDepthWrite,
		E_ImageLayoutUndefined, E_ImageLayoutDepth,
		E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment,
		E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment);

	E_RendererStartRender(attachments, 3, render_size, 1);

	// Calculate prev view matrix

	mat4 projection;
	glm_mat4_copy(info->projection, projection);

	static u32 frame_counter = 0;

	vec2 jitter;
	jitter[0] = (Halton(frame_counter, 2) - 0.5f);
	jitter[1] = (Halton(frame_counter, 3) - 0.5f);
	glm_vec2_div(jitter, (vec2) { info->width, info->height }, jitter);

	++frame_counter;

	projection[2][0] = jitter[0];
	projection[2][1] = jitter[1];

	mat4 model_view;
	glm_mat4_mul(projection, info->view, model_view);

	GeometryUniforms upload_uniforms;
	glm_mat4_copy(info->projection, upload_uniforms.projection);
	glm_mat4_copy(info->view, upload_uniforms.view);
	glm_mat4_copy(data->uniforms.view, upload_uniforms.prev_view);
	glm_vec3_copy(data->uniforms.camera_position, upload_uniforms.camera_position);

	// Draw skybox
	// Calculate skybox model view projection matrix
	if (data->skybox_enabled)
	{
		mat4 output_matrix;
		glm_mat4_identity(output_matrix);

		mat4 view_mat3;
		glm_mat4_copy(info->view, view_mat3);

		view_mat3[3][0] = 0.0f;
		view_mat3[3][1] = 0.0f;
		view_mat3[3][2] = 0.0f;
		view_mat3[0][3] = 0.0f;
		view_mat3[1][3] = 0.0f;
		view_mat3[2][3] = 0.0f;
		view_mat3[3][3] = 1.0f;

		mat4 scale_matrix;
		glm_mat4_identity(scale_matrix);
		glm_scale(scale_matrix, (vec3) { 100000.0f, 100000.0f, 100000.0f });

		glm_mat4_mul(output_matrix, projection, output_matrix);
		glm_mat4_mul(output_matrix, view_mat3, output_matrix);
		glm_mat4_mul(output_matrix, scale_matrix, output_matrix);

		E_BindMaterial(data->skybox_material->as.material);
		E_MaterialPushConstants(data->skybox_material->as.material, &output_matrix, sizeof(output_matrix));
		E_BindMaterialInstance(data->skybox_instance, data->skybox_material->as.material, 0);
		for (i32 i = 0; i < data->skybox_mesh->as.mesh->submesh_count; i++)
		{
			E_Submesh submesh = data->skybox_mesh->as.mesh->submeshes[i];

			E_BindBuffer(submesh.vertex_buffer);
			E_BindBuffer(submesh.index_buffer);
			E_DrawIndexed(0, submesh.index_count);
		}
	}

	// Draw meshes
	E_BindMaterial(data->geometry_material->as.material);
	E_MaterialPushConstants(data->geometry_material->as.material, &upload_uniforms, sizeof(GeometryUniforms));
	E_SetBufferData(data->light_buffer, info->point_lights, sizeof(info->point_lights));

	for (i32 i = 0; i < info->drawable_count; i++)
	{
		E_Drawable drawable = info->drawables[i];

		E_BindMaterialInstance(drawable.material_instance, data->geometry_material->as.material, 0);
		E_BindMaterialInstance(data->light_material_instance, data->geometry_material->as.material, 1);
		for (i32 i = 0; i < drawable.mesh->submesh_count; i++)
		{
			E_Submesh submesh = drawable.mesh->submeshes[i];
			E_BindBuffer(submesh.vertex_buffer);
			E_BindBuffer(submesh.index_buffer);
			E_DrawIndexed(0, submesh.index_count);
		}
	}

	E_RendererEndRender();

	E_ImageTransitionLayout(node->outputs[1],
		E_ImageAccessColorWrite, E_ImageAccessShaderRead,
		E_ImageLayoutColor, E_ImageLayoutShaderRead,
		E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader);

	E_ImageTransitionLayout(node->outputs[0],
		E_ImageAccessColorWrite, E_ImageAccessShaderRead,
		E_ImageLayoutColor, E_ImageLayoutShaderRead,
		E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader);

	glm_mat4_copy(info->view, data->uniforms.view);
}

void GeometryNodeResize(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_FreeImage(node->outputs[2]);
	E_FreeImage(node->outputs[1]);
	E_FreeImage(node->outputs[0]);
	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);
	node->outputs[1] = E_MakeImage(info->width, info->height, E_ImageFormatRG16, E_ImageUsageRenderGraphNodeOutput);
	node->outputs[2] = E_MakeImage(info->width, info->height, E_ImageFormatD32_Float, E_ImageUsageDepthStencilAttachment | E_ImageUsageSampled);
	data->first_render = 1;
}

E_RenderGraphNode* CreateGeometryNode()
{
	E_RenderGraphNode* node = malloc(sizeof(E_RenderGraphNode));

	node->enabled = 1;
	node->init_func = GeometryNodeInit;
	node->clean_func = GeometryNodeClean;
	node->resize_func = GeometryNodeResize;
	node->execute_func = GeometryNodeExecute;
	node->name = "GeometryNode";
	node->node_data = malloc(sizeof(GeometryData));
	
	node->input_count = 0;
	memset(node->inputs, 0, sizeof(node->inputs));

	return node;
}

E_Material* GetGeometryNodeMaterial(E_RenderGraphNode* node)
{
	GeometryData* data = (GeometryData*)node->node_data;
	assert(data);

	return data->geometry_material->as.material;
}

void GeometryNodeDrawGUI(E_RenderGraphNode* node)
{
	GeometryData* data = (GeometryData*)node->node_data;

	b32 geometry_node = igTreeNodeEx_Str("Geometry Node", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
	if (geometry_node)
	{
		igCheckbox("Enable Skybox", &data->skybox_enabled);
		igTreePop();
	}
}
