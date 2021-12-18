#include "Material.h"

#ifdef EUPHORBE_WINDOWS
	#include <Euphorbe/Graphics/Vulkan/VulkanMaterial.h>
#endif

E_Material* E_CreateMaterial(E_MaterialCreateInfo* create_info)
{
	E_Material* result = malloc(sizeof(E_Material));
	result->material_create_info = create_info;
	
#ifdef EUPHORBE_WINDOWS
	result->rhi_handle = E_Vk_CreateMaterial(create_info);
#endif

	return result;
}

void E_FreeMaterial(E_Material* material)
{
#ifdef EUPHORBE_WINDOWS
	E_Vk_FreeMaterial((E_VulkanMaterial*)material->rhi_handle);
#endif
	free(material);
}