#include "Material.h"

#pragma warning(disable: 6011)
#pragma warning(disable: 6387)
#pragma warning(disable: 4244)
#pragma warning(disable: 4267)
#pragma warning(disable: 4996)

#ifdef EUPHORBE_WINDOWS
	#include <Euphorbe/Graphics/Vulkan/VulkanMaterial.h>
#endif

#include <toml.h>
#include <Euphorbe/Core/Log.h>

E_CullMode E_GetCullModeFromString(const char* str);
E_CompareOP E_GetCompareOPFromString(const char* str);
E_FrontFace E_GetFrontFaceFromString(const char* str);
E_PrimitiveTopology E_GetPrimitiveTopologyFromString(const char* str);
E_PolygonMode E_GetPolygonModeFromString(const char* str);
E_ImageFormat E_GetImageFormatFromString(const char* str);
E_DescriptorType E_GetDescriptorTypeFromString(const char* str);

E_Material* E_CreateMaterial(E_MaterialCreateInfo* create_info)
{
	E_Material* result = malloc(sizeof(E_Material));
	memset(result, 0, sizeof(E_Material));
	result->loaded_from_file = 0;
	result->material_create_info = create_info;
	
#ifdef EUPHORBE_WINDOWS
	result->rhi_handle = E_Vk_CreateMaterial(create_info);
#endif

	return result;
}

E_Material* E_CreateMaterialFromFile(const char* path)
{
	E_Material* material = malloc(sizeof(E_Material));
	memset(material, 0, sizeof(E_Material));
	material->loaded_from_file = 1;
	material->material_create_info = malloc(sizeof(E_MaterialCreateInfo));
	memset(material->material_create_info, 0, sizeof(E_MaterialCreateInfo));

	FILE* fp = NULL;
	char errbuf[200] = {0};

	fp = fopen(path, "r");
	if (!fp)
		E_LogError("MATERIAL READ ERROR: Failed to read file (path=%s)", path);
	
	toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
	fclose(fp);

	if (!conf)
		E_LogError("TOML READ ERROR: Failed to parse - %s", errbuf);

	toml_table_t* material_properties = toml_table_in(conf, "MaterialProperties");
	if (!material_properties)
		E_LogError("TOML READ ERROR: Failed to parse material properties - %s", errbuf);

	toml_table_t* render_info = toml_table_in(conf, "RenderInfo");
	if (!render_info)
		E_LogError("TOML READ ERROR: Failed to parse render info - %s", errbuf);

	toml_table_t* shaders = toml_table_in(conf, "Shaders");
	if (!shaders)
		E_LogError("TOML READ ERROR: Failed to parse shaders - %s", errbuf);

	toml_table_t* descriptor_layout = toml_table_in(conf, "DescriptorLayout");
	if (!descriptor_layout)
		E_LogError("TOML READ ERROR: Failed to parse descriptor layout - %s", errbuf);

	toml_table_t* push_constants = toml_table_in(conf, "PushConstants");
	if (!push_constants)
		E_LogError("TOML READ ERROR: Failed to parse push constants - %s", errbuf);

	// MaterialProperties

	toml_datum_t cull_mode = toml_string_in(material_properties, "CullMode");
	toml_datum_t depth_operation = toml_string_in(material_properties, "DepthOperation");
	toml_datum_t front_face = toml_string_in(material_properties, "FrontFace");
	toml_datum_t primitive_topology = toml_string_in(material_properties, "PrimitiveTopology");
	toml_datum_t polygon_mode = toml_string_in(material_properties, "PolygonMode");

	assert(cull_mode.ok);
	assert(depth_operation.ok);
	assert(front_face.ok);
	assert(primitive_topology.ok);
	assert(polygon_mode.ok);

	material->material_create_info->cull_mode = E_GetCullModeFromString(cull_mode.u.s);
	material->material_create_info->depth_op = E_GetCompareOPFromString(depth_operation.u.s);
	material->material_create_info->front_face = E_GetFrontFaceFromString(front_face.u.s);
	material->material_create_info->primitive_topology = E_GetPrimitiveTopologyFromString(primitive_topology.u.s);
	material->material_create_info->polygon_mode = E_GetPolygonModeFromString(polygon_mode.u.s);

	free(cull_mode.u.s);
	free(depth_operation.u.s);
	free(front_face.u.s);
	free(primitive_topology.u.s);
	free(polygon_mode.u.s);

	// Render Info

	toml_datum_t attachment_count = toml_int_in(render_info, "ColorAttachmentCount");
	toml_datum_t depth_format = toml_string_in(render_info, "DepthFormat");
	toml_array_t* attachment_formats = toml_array_in(render_info, "ColorFormats");
	
	assert(attachment_count.ok);
	assert(depth_format.ok);

	material->material_create_info->render_info.color_attachment_count = attachment_count.u.i;
	material->material_create_info->render_info.depth_format = E_GetImageFormatFromString(depth_format.u.s);
	for (i32 i = 0; i < material->material_create_info->render_info.color_attachment_count; i++)
	{
		toml_datum_t data = toml_string_at(attachment_formats, i);
		char* format = data.u.s;
		material->material_create_info->render_info.color_formats[i] = E_GetImageFormatFromString(format);
		free(format);
	}

	free(depth_format.u.s);

	// Shaders

	toml_datum_t mesh_shader_enabled = toml_bool_in(shaders, "EnableMeshShaders");
	assert(mesh_shader_enabled.ok);
	material->material_create_info->mesh_shader_enabled = mesh_shader_enabled.u.b;
	
	if (!mesh_shader_enabled.u.b)
	{
		toml_datum_t vertex_path = toml_string_in(shaders, "Vertex");
		toml_datum_t fragment_path = toml_string_in(shaders, "Fragment");

		assert(vertex_path.ok);
		assert(fragment_path.ok);

		material->material_create_info->vertex_shader = E_LoadResource(vertex_path.u.s, E_ResourceTypeVertexShader);
		material->material_create_info->fragment_shader = E_LoadResource(fragment_path.u.s, E_ResourceTypeFragmentShader);

		free(vertex_path.u.s);
		free(fragment_path.u.s);
	}
	else
	{
		toml_datum_t task_path = toml_string_in(shaders, "Task");
		toml_datum_t mesh_path = toml_string_in(shaders, "Mesh");
		toml_datum_t fragment_path = toml_string_in(shaders, "Fragment");

		assert(task_path.ok && mesh_path.ok && fragment_path.ok);

		material->material_create_info->task_shader = E_LoadResource(task_path.u.s, E_ResourceTypeTaskShader);
		material->material_create_info->mesh_shader = E_LoadResource(mesh_path.u.s, E_ResourceTypeMeshShader);
		material->material_create_info->fragment_shader = E_LoadResource(fragment_path.u.s, E_ResourceTypeFragmentShader);

		free(task_path.u.s);
		free(mesh_path.u.s);
		free(fragment_path.u.s);
	}

	// Descriptor layout

	toml_datum_t descriptor_set_layout_count = toml_int_in(descriptor_layout, "DescriptorSetLayoutCount");
	toml_array_t* descriptor_set_layouts = toml_array_in(descriptor_layout, "DescriptorSetLayouts");
	assert(descriptor_set_layout_count.ok);

	material->material_create_info->descriptor_set_layout_count = descriptor_set_layout_count.u.i;
	for (i32 i = 0; i < material->material_create_info->descriptor_set_layout_count; i++)
	{
		// Get descriptor set layout info (descriptors, descriptor_count)
		toml_array_t* data = toml_array_at(descriptor_set_layouts, i);
		toml_array_t* descriptors = toml_array_at(data, 0);
		toml_datum_t descriptor_count = toml_int_at(data, 1);
		assert(descriptor_count.ok);

		material->material_create_info->descriptor_set_layouts[i].descriptor_count = descriptor_count.u.i;

		for (i32 j = 0; j < material->material_create_info->descriptor_set_layouts[i].descriptor_count; j++)
		{
			toml_datum_t descriptor_type = toml_string_at(descriptors, j);
			assert(descriptor_type.ok);
			material->material_create_info->descriptor_set_layouts[i].descriptors[j].type = E_GetDescriptorTypeFromString(descriptor_type.u.s);
			material->material_create_info->descriptor_set_layouts[i].descriptors[j].binding = j;
			free(descriptor_type.u.s);
		}
	}

	// Push Constants
	toml_datum_t has_push_constants = toml_int_in(push_constants, "HasPushConstants");
	toml_datum_t push_constants_size = toml_int_in(push_constants, "Size");
	assert(has_push_constants.ok);
	assert(push_constants_size.ok);
	material->material_create_info->has_push_constants = has_push_constants.u.i;
	material->material_create_info->push_constants_size = push_constants_size.u.i;

	// Finally, allocate the vulkan handle
#ifdef EUPHORBE_WINDOWS
	material->rhi_handle = E_Vk_CreateMaterial(material->material_create_info);
#endif

	// Free the toml stuff
	toml_free(conf);

	return material;
}

E_Material* E_CreateComputeMaterialFromFile(const char* path)
{
	E_Material* material = malloc(sizeof(E_Material));
	memset(material, 0, sizeof(E_Material));
	material->loaded_from_file = 1;
	material->material_create_info = malloc(sizeof(E_MaterialCreateInfo));
	memset(material->material_create_info, 0, sizeof(E_MaterialCreateInfo));

	FILE* fp = NULL;
	char errbuf[200] = { 0 };

	fp = fopen(path, "r");
	if (!fp)
		E_LogError("MATERIAL READ ERROR: Failed to read file (path=%s)", path);

	toml_table_t* conf = toml_parse_file(fp, errbuf, sizeof(errbuf));
	fclose(fp);

	if (!conf)
		E_LogError("TOML READ ERROR: Failed to parse - %s", errbuf);

	toml_table_t* shaders = toml_table_in(conf, "Shaders");
	if (!shaders)
		E_LogError("TOML READ ERROR: Failed to parse shaders - %s", errbuf);

	toml_table_t* descriptor_layout = toml_table_in(conf, "DescriptorLayout");
	if (!descriptor_layout)
		E_LogError("TOML READ ERROR: Failed to parse descriptor layout - %s", errbuf);

	toml_table_t* push_constants = toml_table_in(conf, "PushConstants");
	if (!push_constants)
		E_LogError("TOML READ ERROR: Failed to parse push constants - %s", errbuf);

	// Shaders

	toml_datum_t compute_path = toml_string_in(shaders, "Compute");
	material->material_create_info->compute_shader = E_LoadResource(compute_path.u.s, E_ResourceTypeComputeShader);
	free(compute_path.u.s);

	// Descriptor layout

	toml_datum_t descriptor_set_layout_count = toml_int_in(descriptor_layout, "DescriptorSetLayoutCount");
	toml_array_t* descriptor_set_layouts = toml_array_in(descriptor_layout, "DescriptorSetLayouts");
	assert(descriptor_set_layout_count.ok);

	material->material_create_info->descriptor_set_layout_count = descriptor_set_layout_count.u.i;
	for (i32 i = 0; i < material->material_create_info->descriptor_set_layout_count; i++)
	{
		// Get descriptor set layout info (descriptors, descriptor_count)
		toml_array_t* data = toml_array_at(descriptor_set_layouts, i);
		toml_array_t* descriptors = toml_array_at(data, 0);
		toml_datum_t descriptor_count = toml_int_at(data, 1);
		assert(descriptor_count.ok);

		material->material_create_info->descriptor_set_layouts[i].descriptor_count = descriptor_count.u.i;

		for (i32 j = 0; j < material->material_create_info->descriptor_set_layouts[i].descriptor_count; j++)
		{
			toml_datum_t descriptor_type = toml_string_at(descriptors, j);
			assert(descriptor_type.ok);
			material->material_create_info->descriptor_set_layouts[i].descriptors[j].type = E_GetDescriptorTypeFromString(descriptor_type.u.s);
			material->material_create_info->descriptor_set_layouts[i].descriptors[j].binding = j;
			free(descriptor_type.u.s);
		}
	}

	// Push Constants
	toml_datum_t has_push_constants = toml_int_in(push_constants, "HasPushConstants");
	toml_datum_t push_constants_size = toml_int_in(push_constants, "Size");
	assert(has_push_constants.ok);
	assert(push_constants_size.ok);
	material->material_create_info->has_push_constants = has_push_constants.u.i;
	material->material_create_info->push_constants_size = push_constants_size.u.i;

	// Finally, allocate the vulkan handle
#ifdef EUPHORBE_WINDOWS
	material->rhi_handle = E_Vk_CreateComputeMaterial(material->material_create_info);
#endif

	// Free the toml stuff
	toml_free(conf);

	return material;
}

void E_FreeMaterial(E_Material* material)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_FreeMaterial((E_VulkanMaterial*)material->rhi_handle);
#endif
	if (material->loaded_from_file)
	{
		if (material->material_create_info->vertex_shader) E_FreeResource(material->material_create_info->vertex_shader);
		if (material->material_create_info->fragment_shader) E_FreeResource(material->material_create_info->fragment_shader);
		if (material->material_create_info->compute_shader) E_FreeResource(material->material_create_info->compute_shader);
		if (material->material_create_info->mesh_shader) E_FreeResource(material->material_create_info->mesh_shader);
		if (material->material_create_info->task_shader) E_FreeResource(material->material_create_info->task_shader);
		free(material->material_create_info);
	}
	free(material);
}

E_MaterialInstance* E_CreateMaterialInstance(E_Material* material, i32 set_layout_index)
{
	E_MaterialInstance* instance = malloc(sizeof(E_MaterialInstance));
	instance->material = material;
	
#ifdef EUPHORBE_WINDOWS
	instance->rhi_handle = E_Vk_CreateMaterialInstance((E_VulkanMaterial*)material->rhi_handle, set_layout_index);
#endif

	return instance;
}

void E_MaterialInstanceWriteBuffer(E_MaterialInstance* instance, i32 binding, E_Buffer* buffer, i64 buffer_size)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_MaterialInstanceWriteBuffer((E_VulkanMaterialInstance*)instance->rhi_handle, binding, (E_VulkanBuffer*)buffer->rhi_handle, buffer_size);
#endif
}

void E_MaterialInstanceWriteStorageBuffer(E_MaterialInstance* instance, i32 binding, E_Buffer* buffer, i64 buffer_size)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_MaterialInstanceWriteStorageBuffer(instance->rhi_handle, binding, buffer->rhi_handle, buffer_size);
#endif
}

void E_MaterialInstanceWriteSampler(E_MaterialInstance* instance, i32 binding, E_Sampler* sampler)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_MaterialInstanceWriteSampler(instance->rhi_handle, binding, sampler->rhi_handle);
#endif
}

void E_MaterialInstanceWriteSampledImage(E_MaterialInstance* instance, i32 binding, E_Image* image)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_MaterialInstanceWriteSampledImage(instance->rhi_handle, binding, image->rhi_handle);
#endif
}

void E_MaterialInstanceWriteImage(E_MaterialInstance* instance, i32 binding, E_Image* image, E_Sampler* sampler)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_MaterialInstanceWriteImage((E_VulkanMaterialInstance*)instance->rhi_handle, binding, (E_VulkanImage*)image->rhi_handle, (E_VulkanSampler*)sampler->rhi_handle);
#endif
}

void E_MaterialInstanceWriteStorageImage(E_MaterialInstance* instance, i32 binding, E_Image* image, E_Sampler* sampler)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_MaterialInstanceWriteStorageImage((E_VulkanMaterialInstance*)instance->rhi_handle, binding, (E_VulkanImage*)image->rhi_handle, (E_VulkanSampler*)sampler->rhi_handle);
#endif
}

void E_FreeMaterialInstance(E_MaterialInstance* instance)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_FreeMaterialInstance((E_VulkanMaterialInstance*)instance->rhi_handle);
#endif
	free(instance);
}

E_CullMode E_GetCullModeFromString(const char* str)
{
	if (!strcmp(str, "None"))
		return E_CullModeNone;
	if (!strcmp(str, "Front"))
		return E_CullModeFront;
	if (!strcmp(str, "Back"))
		return E_CullModeBack;
	if (!strcmp(str, "FrontBack"))
		return E_CullModeFrontAndBack;

	return E_CullModeNone;
}

E_CompareOP E_GetCompareOPFromString(const char* str)
{
	if (!strcmp(str, "Never"))
		return E_CompareOPNever;
	if (!strcmp(str, "Less"))
		return E_CompareOPLess;
	if (!strcmp(str, "Equal"))
		return E_CompareOPEqual;
	if (!strcmp(str, "LessEqual"))
		return E_CompareOPLessEqual;
	if (!strcmp(str, "Greater"))
		return E_CompareOPGreater;
	if (!strcmp(str, "NotEqual"))
		return E_CompareOPNotEqual;
	if (!strcmp(str, "GreaterEqual"))
		return E_CompareOPGreaterEqual;
	if (!strcmp(str, "Always"))
		return E_CompareOPAlways;

	return E_CompareOPNever;
}

E_FrontFace E_GetFrontFaceFromString(const char* str)
{
	if (!strcmp(str, "CW"))
		return E_FrontFaceCW;
	if (!strcmp(str, "CCW"))
		return E_FrontFaceCCW;

	return E_FrontFaceCW;
}

E_PrimitiveTopology E_GetPrimitiveTopologyFromString(const char* str)
{
	if (!strcmp(str, "PointList"))
		return E_PrimitiveTopologyPointList;
	if (!strcmp(str, "LineList"))
		return E_PrimitiveTopologyLineList;
	if (!strcmp(str, "LineStrip"))
		return E_PrimitiveTopologyLineStrip;
	if (!strcmp(str, "TriangleList"))
		return E_PrimitiveTopologyTriangleList;
	if (!strcmp(str, "TriangleStrip"))
		return E_PrimitiveTopologyTriangleStrip;

	return E_PrimitiveTopologyTriangleList;
}

E_PolygonMode E_GetPolygonModeFromString(const char* str)
{
	if (!strcmp(str, "Fill"))
		return E_PolygonModeFill;
	if (!strcmp(str, "Line"))
		return E_PolygonModeLine;
	if (!strcmp(str, "Point"))
		return E_PolygonModePoint;

	return E_PolygonModeFill;
}

E_ImageFormat E_GetImageFormatFromString(const char* str)
{
	if (!strcmp(str, "RGBA8"))
		return E_ImageFormatRGBA8;
	if (!strcmp(str, "RG16"))
		return E_ImageFormatRG16;
	if (!strcmp(str, "RGBA16"))
		return E_ImageFormatRGBA16;
	if (!strcmp(str, "RGBA32"))
		return E_ImageFormatRGBA32;
	if (!strcmp(str, "D32_Float"))
		return E_ImageFormatD32_Float;

	return 0;
}

E_DescriptorType E_GetDescriptorTypeFromString(const char* str)
{
	if (!strcmp(str, "UniformBuffer"))
		return E_DescriptorTypeUniformBuffer;
	if (!strcmp(str, "CombinedImageSampler"))
		return E_DescriptorTypeCombinedImageSampler;
	if (!strcmp(str, "StorageImage"))
		return E_DescriptorTypeStorageImage;
	if (!strcmp(str, "SampledImage"))
		return E_DescriptorTypeSampledImage;
	if (!strcmp(str, "Sampler"))
		return E_DescriptorTypeSampler;
	if (!strcmp(str, "StorageBuffer"))
		return E_DescriptorTypeStorageBuffer;

	return 0;
}
