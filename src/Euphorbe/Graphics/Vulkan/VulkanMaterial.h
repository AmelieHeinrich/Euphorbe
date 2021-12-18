#ifndef EUPHORBE_VULKAN_MATERIAL_H
#define EUPHORBE_VULKAN_MATERIAL_H

#include <Euphorbe/Graphics/Material.h>
#include <vulkan/vulkan.h>

typedef struct E_VulkanMaterial E_VulkanMaterial;
struct E_VulkanMaterial
{
    VkPipeline pipeline;
    VkPipelineLayout pipeline_layout;
};

E_VulkanMaterial* E_Vk_CreateMaterial(E_MaterialCreateInfo* create_info);
void E_Vk_FreeMaterial(E_VulkanMaterial* material);

#endif