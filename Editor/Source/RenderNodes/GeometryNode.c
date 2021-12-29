#include "GeometryNode.h"

#include <Euphorbe/Graphics/Renderer.h>

typedef struct GeometryData GeometryData;
struct GeometryData
{
	b32 first_render;
	b32 skybox_enabled;
	E_Image* depth_buffer;

	E_ResourceFile* geometry_material;
	E_ResourceFile* skybox_material;

	E_ResourceFile* skybox_mesh;
	E_MaterialInstance* skybox_instance;
	E_ResourceFile* hdr_skybox;
};

void GeometryNodeInit(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	node->output = E_MakeImage(info->width, info->height, E_ImageFormatRGBA8);
	data->depth_buffer = E_MakeImage(info->width, info->height, E_ImageFormatD32_Float);
	data->first_render = 1;
	data->geometry_material = E_LoadResource("Assets/Materials/MeshMaterial.toml", E_ResourceTypeMaterial);

	data->skybox_material = E_LoadResource("Assets/Materials/SkyboxMaterial.toml", E_ResourceTypeMaterial);
	data->skybox_mesh = E_LoadResource("Assets/Models/Cube.gltf", E_ResourceTypeMesh);
	data->skybox_instance = E_CreateMaterialInstance(data->skybox_material->as.material);
	data->hdr_skybox = E_MakeHDRImageFromFile("Assets/EnvMaps/SnowyField/4k.hdr");
	data->skybox_enabled = 1;

	E_MaterialInstanceWriteImage(data->skybox_instance, 0, data->hdr_skybox);
}

void GeometryNodeClean(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_FreeResource(data->skybox_mesh);
	E_FreeImage(data->hdr_skybox);
	E_FreeMaterialInstance(data->skybox_instance);
	E_FreeResource(data->skybox_material);

	E_FreeImage(node->output);
	E_FreeImage(data->depth_buffer);
	E_FreeResource(data->geometry_material);
	free(data);
}

void GeometryNodeExecute(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_ClearValue color_clear = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0 };
	E_ClearValue depth_clear = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };

	E_ImageAttachment attachments[2] = {
		{ node->output, E_ImageLayoutColor, color_clear },
		{ data->depth_buffer, E_ImageLayoutDepth, depth_clear }
	};

	E_ImageLayout src_render_buffer_image_layout = data->first_render ? E_ImageLayoutUndefined : E_ImageLayoutShaderRead;
	data->first_render = 0;

	vec2 render_size = { info->width, info->height };

	E_ImageTransitionLayout(node->output,
		E_ImageAccessShaderRead, E_ImageAccessColorWrite,
		src_render_buffer_image_layout, E_ImageLayoutColor,
		E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput);

	E_ImageTransitionLayout(data->depth_buffer,
		0, E_ImageAccessDepthWrite,
		E_ImageLayoutUndefined, E_ImageLayoutDepth,
		E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment,
		E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment);

	E_RendererStartRender(attachments, 2, render_size, 1);

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
		glm_scale(scale_matrix, (vec3) { 10000.0f, 10000.0f, 10000.0f });

		glm_mat4_mul(output_matrix, info->projection, output_matrix);
		glm_mat4_mul(output_matrix, view_mat3, output_matrix);
		glm_mat4_mul(output_matrix, scale_matrix, output_matrix);

		E_BindMaterial(data->skybox_material->as.material);
		E_MaterialPushConstants(data->skybox_material->as.material, &output_matrix, sizeof(output_matrix));
		E_BindMaterialInstance(data->skybox_instance, data->skybox_material->as.material);
		for (i32 i = 0; i < data->skybox_mesh->as.mesh->submesh_count; i++)
		{
			E_Submesh submesh = data->skybox_mesh->as.mesh->submeshes[i];

			E_BindBuffer(submesh.vertex_buffer);
			E_BindBuffer(submesh.index_buffer);
			E_DrawIndexed(0, submesh.index_count);
		}
	}

	// Draw meshes

	mat4 model_view;
	glm_mat4_mul(info->projection, info->view, model_view);
	E_BindMaterial(data->geometry_material->as.material);
	E_MaterialPushConstants(data->geometry_material->as.material, &model_view, sizeof(mat4));

	for (i32 i = 0; i < info->drawable_count; i++)
	{
		E_Drawable drawable = info->drawables[i];

		E_BindMaterialInstance(drawable.material_instance, data->geometry_material->as.material);
		for (i32 i = 0; i < drawable.mesh->submesh_count; i++)
		{
			E_Submesh submesh = drawable.mesh->submeshes[i];
			E_BindBuffer(submesh.vertex_buffer);
			E_BindBuffer(submesh.index_buffer);
			E_DrawIndexed(0, submesh.index_count);
		}
	}

	E_RendererEndRender();

	E_ImageTransitionLayout(node->output,
		E_ImageAccessColorWrite, E_ImageAccessShaderRead,
		E_ImageLayoutColor, E_ImageLayoutShaderRead,
		E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader);
}

void GeometryNodeResize(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_FreeImage(node->output);
	E_FreeImage(data->depth_buffer);
	node->output = E_MakeImage(info->width, info->height, E_ImageFormatRGBA8);
	data->depth_buffer = E_MakeImage(info->width, info->height, E_ImageFormatD32_Float);
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

	return node;
}

E_Material* GetGeometryNodeMaterial(E_RenderGraphNode* node)
{
	GeometryData* data = (GeometryData*)node->node_data;

	return data->geometry_material->as.material;
}

void EnableGeometryNodeSkybox(E_RenderGraphNode* node, b32 enable)
{
	GeometryData* data = (GeometryData*)node->node_data;

	data->skybox_enabled = enable;
}
