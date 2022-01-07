#ifndef EUPHORBE_VULKAN_MATERIAL_H
#define EUPHORBE_VULKAN_MATERIAL_H

#include <Euphorbe/Graphics/Material.h>
#include <vulkan/vulkan.h>

#include "VulkanBuffer.h"
#include "VulkanImage.h"

typedef struct E_VulkanMaterial E_VulkanMaterial;
struct E_VulkanMaterial
{
    VkPipeline pipeline;
    VkPipelineLayout pipeline_layout;
    
    VkDescriptorSetLayout set_layouts[EUPHORBE_MAX_DESCRIPTORS];
    i32 set_layout_count;
};

typedef struct E_VulkanMaterialInstance E_VulkanMaterialInstance;
struct E_VulkanMaterialInstance
{
    VkDescriptorSet set;
};

E_VulkanMaterial* E_Vk_CreateMaterial(E_MaterialCreateInfo* create_info);
void E_Vk_FreeMaterial(E_VulkanMaterial* material);
E_VulkanMaterial* E_Vk_CreateComputeMaterial(E_MaterialCreateInfo* create_info);

E_VulkanMaterialInstance* E_Vk_CreateMaterialInstance(E_VulkanMaterial* material, i32 set_layout_index);
void E_Vk_MaterialInstanceWriteBuffer(E_VulkanMaterialInstance* instance, i32 binding, E_VulkanBuffer* buffer, i32 buffer_size);
void E_Vk_MaterialInstanceWriteSampler(E_VulkanMaterialInstance* instance, i32 binding, E_VulkanSampler* sampler);
void E_Vk_MaterialInstanceWriteSampledImage(E_VulkanMaterialInstance* instance, i32 binding, E_VulkanImage* image);
void E_Vk_MaterialInstanceWriteImage(E_VulkanMaterialInstance* instance, i32 binding, E_VulkanImage* image, E_VulkanSampler* sampler);
void E_Vk_MaterialInstanceWriteStorageImage(E_VulkanMaterialInstance* instance, i32 binding, E_VulkanImage* image, E_VulkanSampler* sampler);
void E_Vk_FreeMaterialInstance(E_VulkanMaterialInstance* instance);

#endif