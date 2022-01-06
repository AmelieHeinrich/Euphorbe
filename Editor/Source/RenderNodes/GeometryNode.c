#include "GeometryNode.h"

#include <Euphorbe/Graphics/Renderer.h>
#include <Euphorbe/Resource/Resource.h>

typedef struct GeometryUniforms GeometryUniforms;
struct GeometryUniforms
{
	mat4 projection;
	mat4 view;
	vec3 camera_position;
	f32 padding;
};

typedef struct GeometryData GeometryData;
struct GeometryData
{
	b32 first_render;
	b32 skybox_should_layout;
	b32 skybox_enabled;

	E_Buffer* light_buffer;

	E_ResourceFile* geometry_material;
	E_ResourceFile* equirectangular_cubemap_material;
	E_ResourceFile* skybox_material;
	E_ResourceFile* irradiance_material;
	E_ResourceFile* prefilter_material;
	E_ResourceFile* brdf_material;

	E_MaterialInstance* equirectangular_cubemap_instance;
	E_MaterialInstance* skybox_instance;
	E_MaterialInstance* irradiance_instance;
	E_MaterialInstance* prefilter_instance;
	E_MaterialInstance* brdf_instance;
	E_MaterialInstance* light_material_instance;

	E_ResourceFile* skybox_mesh;

	E_Image* hdr_skybox_texture;
	E_Image* cubemap;
	E_Image* irradiance;
	E_Image* prefilter;
	E_Image* brdf;

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
	node->outputs[1] = E_MakeImage(info->width, info->height, E_ImageFormatD32_Float, E_ImageUsageDepthStencilAttachment | E_ImageUsageSampled);
	node->output_count = 2;

	data->first_render = 1;
	data->skybox_should_layout = 1;

	data->skybox_mesh = E_LoadResource("Assets/Models/Cube.gltf", E_ResourceTypeMesh);
	data->skybox_enabled = E_GetCVar(info->cvar_table_ptr, "enable_skybox").u.b;

	data->geometry_material = E_LoadResource("Assets/Materials/GeometryMaterial.toml", E_ResourceTypeMaterial);
	data->skybox_material = E_LoadResource("Assets/Materials/SkyboxMaterial.toml", E_ResourceTypeMaterial);
	data->equirectangular_cubemap_material = E_LoadResource("Assets/Materials/EquirectangularCubemapMaterial.toml", E_ResourceTypeComputeMaterial);
	data->irradiance_material = E_LoadResource("Assets/Materials/IrradianceMaterial.toml", E_ResourceTypeComputeMaterial);
	data->prefilter_material = E_LoadResource("Assets/Materials/PrefilterMaterial.toml", E_ResourceTypeComputeMaterial);
	data->brdf_material = E_LoadResource("Assets/Materials/BRDFMaterial.toml", E_ResourceTypeComputeMaterial);

	data->skybox_instance = E_CreateMaterialInstance(data->skybox_material->as.material, 0);
	data->equirectangular_cubemap_instance = E_CreateMaterialInstance(data->equirectangular_cubemap_material->as.material, 0);
	data->irradiance_instance = E_CreateMaterialInstance(data->irradiance_material->as.material, 0);
	data->prefilter_instance = E_CreateMaterialInstance(data->prefilter_material->as.material, 0);
	data->brdf_instance = E_CreateMaterialInstance(data->brdf_material->as.material, 0);

	data->hdr_skybox_texture = E_MakeHDRImageFromFile("Assets/EnvMaps/SnowyField/4k.hdr");
	data->cubemap = E_MakeCubeMap(512, 512, E_ImageFormatRGBA16_Unorm, E_ImageUsageStorage | E_ImageUsageSampled);
	data->irradiance = E_MakeCubeMap(128, 128, E_ImageFormatRGBA16_Unorm, E_ImageUsageStorage | E_ImageUsageSampled);
	data->prefilter = E_MakeCubeMap(512, 512, E_ImageFormatRGBA16_Unorm, E_ImageUsageStorage | E_ImageUsageSampled);
	data->brdf = E_MakeImage(512, 512, E_ImageFormatRG16, E_ImageUsageStorage | E_ImageUsageSampled);

	// Begin compute shader

	E_CommandBuffer* compute_cmd_buf = E_CreateCommandBuffer(E_CommandBufferTypeCompute);
	E_BeginCommandBuffer(compute_cmd_buf);

	// Equi to cubemap

	E_CommandBufferImageTransitionLayout(compute_cmd_buf, data->hdr_skybox_texture, 0, 0, E_ImageLayoutUndefined, E_ImageLayoutGeneral, E_ImagePipelineStageTop, E_ImagePipelineStageBottom, 0);
	E_CommandBufferImageTransitionLayout(compute_cmd_buf, data->cubemap, 0, 0, E_ImageLayoutUndefined, E_ImageLayoutGeneral, E_ImagePipelineStageTop, E_ImagePipelineStageBottom, 0);

	E_MaterialInstanceWriteStorageImage(data->equirectangular_cubemap_instance, 0, data->hdr_skybox_texture);
	E_MaterialInstanceWriteStorageImage(data->equirectangular_cubemap_instance, 1, data->cubemap);

	E_CommandBufferBindComputeMaterial(compute_cmd_buf, data->equirectangular_cubemap_material->as.material);
	E_CommandBufferBindComputeMaterialInstance(compute_cmd_buf, data->equirectangular_cubemap_instance, data->equirectangular_cubemap_material->as.material, 0);
	E_CommandBufferDispatch(compute_cmd_buf, 1024 / 32, 1024 / 32, 6);

	E_SubmitCommandBuffer(compute_cmd_buf);
	E_FreeCommandBuffer(compute_cmd_buf);

	// Irradiance

	compute_cmd_buf = E_CreateCommandBuffer(E_CommandBufferTypeCompute);
	E_BeginCommandBuffer(compute_cmd_buf);

	E_CommandBufferImageTransitionLayout(compute_cmd_buf, data->cubemap, 0, 0, E_ImageLayoutGeneral, E_ImageLayoutShaderRead, E_ImagePipelineStageTop, E_ImagePipelineStageComputeShader, 0);
	E_CommandBufferImageTransitionLayout(compute_cmd_buf, data->irradiance, 0, 0, E_ImageLayoutUndefined, E_ImageLayoutGeneral, E_ImagePipelineStageTop, E_ImagePipelineStageBottom, 0);

	E_MaterialInstanceWriteImage(data->irradiance_instance, 0, data->cubemap);
	E_MaterialInstanceWriteStorageImage(data->irradiance_instance, 1, data->irradiance);

	E_CommandBufferBindComputeMaterial(compute_cmd_buf, data->irradiance_material->as.material);
	E_CommandBufferBindComputeMaterialInstance(compute_cmd_buf, data->irradiance_instance, data->irradiance_material->as.material, 0);
	E_CommandBufferDispatch(compute_cmd_buf, 128 / 32, 128 / 32, 6);

	E_SubmitCommandBuffer(compute_cmd_buf);
	E_FreeCommandBuffer(compute_cmd_buf);

	// Prefilter

	compute_cmd_buf = E_CreateCommandBuffer(E_CommandBufferTypeCompute);
	E_BeginCommandBuffer(compute_cmd_buf);

	E_CommandBufferImageTransitionLayout(compute_cmd_buf, data->prefilter, 0, 0, E_ImageLayoutUndefined, E_ImageLayoutGeneral, E_ImagePipelineStageTop, E_ImagePipelineStageBottom, 0);

	E_MaterialInstanceWriteImage(data->prefilter_instance, 0, data->cubemap);
	E_MaterialInstanceWriteStorageImage(data->prefilter_instance, 1, data->prefilter);

	E_CommandBufferBindComputeMaterial(compute_cmd_buf, data->prefilter_material->as.material);
	E_CommandBufferBindComputeMaterialInstance(compute_cmd_buf, data->prefilter_instance, data->prefilter_material->as.material, 0);
	
	for (u32 i = 0; i < 5; i++)
	{
		u32 mip_width = (u32)(512.0f * pow(0.5f, i));
		u32 mip_height = (u32)(512.0f * pow(0.5f, i));
		f32 roughness = (f32)i / (f32)(5 - 1);

		vec4 roughness_vec;
		glm_vec4_zero(roughness_vec);
		roughness_vec[0] = roughness;

		E_CommandBufferPushConstants(compute_cmd_buf, data->prefilter_material->as.material, &roughness_vec, sizeof(vec4));
		E_CommandBufferDispatch(compute_cmd_buf, mip_width / 32, mip_height / 32, 6);
	}

	E_SubmitCommandBuffer(compute_cmd_buf);
	E_FreeCommandBuffer(compute_cmd_buf);

	// BRDF

	compute_cmd_buf = E_CreateCommandBuffer(E_CommandBufferTypeCompute);
	E_BeginCommandBuffer(compute_cmd_buf);

	E_CommandBufferImageTransitionLayout(compute_cmd_buf, data->brdf, 0, 0, E_ImageLayoutUndefined, E_ImageLayoutGeneral, E_ImagePipelineStageTop, E_ImagePipelineStageBottom, 0);
	E_MaterialInstanceWriteStorageImage(data->brdf_instance, 0, data->brdf);

	E_CommandBufferBindComputeMaterial(compute_cmd_buf, data->brdf_material->as.material);
	E_CommandBufferBindComputeMaterialInstance(compute_cmd_buf, data->brdf_instance, data->brdf_material->as.material, 0);
	E_CommandBufferDispatch(compute_cmd_buf, 512 / 32, 512 / 32, 1);

	E_SubmitCommandBuffer(compute_cmd_buf);
	E_FreeCommandBuffer(compute_cmd_buf);

	//

	E_FreeImage(data->hdr_skybox_texture);

	//

	E_MaterialInstanceWriteImage(data->skybox_instance, 0, data->cubemap);

	data->light_buffer = E_CreateUniformBuffer(sizeof(info->point_lights));
	data->light_material_instance = E_CreateMaterialInstance(data->geometry_material->as.material, 1);
	E_MaterialInstanceWriteBuffer(data->light_material_instance, 0, data->light_buffer, sizeof(info->point_lights));
	E_MaterialInstanceWriteImage(data->light_material_instance, 1, data->cubemap);
	E_MaterialInstanceWriteImage(data->light_material_instance, 2, data->irradiance);
	E_MaterialInstanceWriteImage(data->light_material_instance, 3, data->prefilter);
	E_MaterialInstanceWriteImage(data->light_material_instance, 4, data->brdf);
}

void GeometryNodeClean(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_FreeBuffer(data->light_buffer);
	E_FreeResource(data->skybox_mesh);
	
	E_FreeMaterialInstance(data->light_material_instance);
	E_FreeMaterialInstance(data->brdf_instance);
	E_FreeMaterialInstance(data->prefilter_instance);
	E_FreeMaterialInstance(data->irradiance_instance);
	E_FreeMaterialInstance(data->equirectangular_cubemap_instance);
	E_FreeMaterialInstance(data->skybox_instance);
	
	E_FreeResource(data->brdf_material);
	E_FreeResource(data->prefilter_material);
	E_FreeResource(data->irradiance_material);
	E_FreeResource(data->equirectangular_cubemap_material);
	E_FreeResource(data->skybox_material);
	E_FreeResource(data->geometry_material);

	E_FreeImage(data->brdf);
	E_FreeImage(data->prefilter);
	E_FreeImage(data->irradiance);
	E_FreeImage(data->cubemap);
	E_FreeImage(node->outputs[1]);
	E_FreeImage(node->outputs[0]);

	free(data);
}

void GeometryNodeExecute(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_CommandBuffer* cmd_buf = E_GetSwapchainCommandBuffer();

	E_ClearValue color_clear = { 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0 };
	E_ClearValue depth_clear = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0 };

	E_ImageAttachment attachments[3] = {
		{ node->outputs[0], E_ImageLayoutColor, color_clear},
		{ node->outputs[1], E_ImageLayoutDepth, depth_clear}
	};

	E_ImageLayout src_render_buffer_image_layout = data->first_render ? E_ImageLayoutUndefined : E_ImageLayoutShaderRead;

	vec2 render_size = { (f32)info->width, (f32)info->height };

	E_CommandBufferImageTransitionLayout(cmd_buf,node->outputs[0],
		E_ImageAccessShaderRead, E_ImageAccessColorWrite,
		src_render_buffer_image_layout, E_ImageLayoutColor,
		E_ImagePipelineStageFragmentShader, E_ImagePipelineStageColorOutput,
		0);

	E_CommandBufferImageTransitionLayout(cmd_buf, node->outputs[1],
		0, E_ImageAccessDepthWrite,
		E_ImageLayoutUndefined, E_ImageLayoutDepth,
		E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment,
		E_ImagePipelineStageEarlyFragment | E_ImagePipelineStageLateFragment,
		0);

	E_CommandBufferSetViewport(cmd_buf, info->width, info->height);
	E_CommandBufferStartRender(cmd_buf, attachments, 2, render_size, 1);

	// Calculate prev view matrix

	mat4 projection;
	glm_mat4_copy(info->projection, projection);

	mat4 model_view;
	glm_mat4_mul(projection, info->view, model_view);

	GeometryUniforms upload_uniforms;
	glm_mat4_copy(info->projection, upload_uniforms.projection);
	glm_mat4_copy(info->view, upload_uniforms.view);
	glm_vec3_copy(info->camera_position, upload_uniforms.camera_position);

	// Transition the layouts of the different environment map textures whether or not the skybox is enabled 
	if (data->skybox_should_layout)
	{
		E_CommandBufferImageTransitionLayout(cmd_buf, data->irradiance, 0, 0, E_ImageLayoutGeneral, E_ImageLayoutShaderRead, E_ImagePipelineStageTop, E_ImagePipelineStageFragmentShader, 0);
		E_CommandBufferImageTransitionLayout(cmd_buf, data->prefilter, 0, 0, E_ImageLayoutGeneral, E_ImageLayoutShaderRead, E_ImagePipelineStageTop, E_ImagePipelineStageFragmentShader, 0);
		E_CommandBufferImageTransitionLayout(cmd_buf, data->brdf, 0, 0, E_ImageLayoutGeneral, E_ImageLayoutShaderRead, E_ImagePipelineStageTop, E_ImagePipelineStageFragmentShader, 0);
		data->skybox_should_layout = 0;
	}

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

		E_CommandBufferBindMaterial(cmd_buf, data->skybox_material->as.material);

		E_CommandBufferPushConstants(cmd_buf, data->skybox_material->as.material, &output_matrix, sizeof(output_matrix));
		E_CommandBufferBindMaterialInstance(cmd_buf, data->skybox_instance, data->skybox_material->as.material, 0);
		for (i32 i = 0; i < data->skybox_mesh->as.mesh->submesh_count; i++)
		{
			E_Submesh submesh = data->skybox_mesh->as.mesh->submeshes[i];

			E_CommandBufferBindBuffer(cmd_buf, submesh.vertex_buffer);
			E_CommandBufferBindBuffer(cmd_buf, submesh.index_buffer);
			E_CommandBufferDrawIndexed(cmd_buf, 0, submesh.index_count);
		}
	}

	// Draw meshes
	E_CommandBufferBindMaterial(cmd_buf, data->geometry_material->as.material);
	E_CommandBufferPushConstants(cmd_buf, data->geometry_material->as.material, &upload_uniforms, sizeof(GeometryUniforms));
	E_SetBufferData(data->light_buffer, info->point_lights, sizeof(info->point_lights));

	for (u32 i = 0; i < info->drawable_count; i++)
	{
		E_Drawable drawable = info->drawables[i];

		E_CommandBufferBindMaterialInstance(cmd_buf, drawable.material_instance, data->geometry_material->as.material, 0);
		E_CommandBufferBindMaterialInstance(cmd_buf, data->light_material_instance, data->geometry_material->as.material, 1);
		for (i32 i = 0; i < drawable.mesh->submesh_count; i++)
		{
			E_Submesh submesh = drawable.mesh->submeshes[i];
			E_CommandBufferBindBuffer(cmd_buf, submesh.vertex_buffer);
			E_CommandBufferBindBuffer(cmd_buf, submesh.index_buffer);
			E_CommandBufferDrawIndexed(cmd_buf, 0, submesh.index_count);
		}
	}

	E_CommandBufferEndRender(cmd_buf);

	E_CommandBufferImageTransitionLayout(cmd_buf, node->outputs[0],
		E_ImageAccessColorWrite, E_ImageAccessShaderRead,
		E_ImageLayoutColor, E_ImageLayoutShaderRead,
		E_ImagePipelineStageColorOutput, E_ImagePipelineStageFragmentShader, 0);

	data->first_render = 0;
}

void GeometryNodeResize(E_RenderGraphNode* node, E_RenderGraphExecuteInfo* info)
{
	GeometryData* data = (GeometryData*)node->node_data;

	E_FreeImage(node->outputs[1]);
	E_FreeImage(node->outputs[0]);
	node->outputs[0] = E_MakeImage(info->width, info->height, E_ImageFormatRGBA16, E_ImageUsageRenderGraphNodeOutput);
	node->outputs[1] = E_MakeImage(info->width, info->height, E_ImageFormatD32_Float, E_ImageUsageDepthStencilAttachment | E_ImageUsageSampled);
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
	memset(node->node_data, 0, sizeof(node->node_data));
	node->input_count = 0;
	memset(node->inputs, 0, sizeof(node->inputs));

	return node;
}

E_ResourceFile* GetGeometryNodeMaterial(E_RenderGraphNode* node)
{
	GeometryData* data = (GeometryData*)node->node_data;
	assert(data);

	return data->geometry_material;
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
