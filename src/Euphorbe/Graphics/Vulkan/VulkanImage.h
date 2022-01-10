#pragma once

#include <Euphorbe/Graphics/Image.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

typedef struct E_VulkanSampler E_VulkanSampler;
struct E_VulkanSampler
{
    VkSamplerAddressMode address_mode;
    VkFilter filter;
    VkSampler sampler;
};

typedef struct E_VulkanImage E_VulkanImage;
struct E_VulkanImage
{
    VkImage image;
    VkImageView image_view;
    VkFormat format;
    VkExtent3D image_extent;
    VkDescriptorSet gui_descriptor_set[FRAMES_IN_FLIGHT];

    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;

    E_ImageFormat euphorbe_format;
    E_ImageUsage euphorbe_usage;
    b32 cube_map;
};

E_VulkanImage* E_Vk_MakeImage(i32 width, i32 height, E_ImageFormat format, E_ImageUsage usage);
E_VulkanImage* E_Vk_MakeImageFromFile(const char* path);
E_VulkanImage* E_Vk_MakeHDRImageFromFile(const char* path);
E_VulkanImage* E_Vk_MakeCubeMap(i32 width, i32 height, E_ImageFormat format, E_ImageUsage usage);
void E_Vk_FreeImage(E_VulkanImage* image);
void E_Vk_ResizeImage(E_VulkanImage* image, i32 width, i32 height);
void E_Vk_DrawImageToGUI(E_VulkanImage* image, i32 width, i32 height, E_VulkanSampler* sampler);

// Samplers
void E_Vk_InitDefaultSamplers();
void E_Vk_FreeDefaultSamplers();

extern E_VulkanSampler* E_Vk_NearestSampler;
extern E_VulkanSampler* E_Vk_LinearSampler;
extern E_VulkanSampler* E_Vk_CubemapSampler;

E_VulkanSampler* E_Vk_CreateSampler(E_ImageAddressMode mode, E_ImageFilter filter);
void E_Vk_FreeSampler(E_VulkanSampler* sampler);