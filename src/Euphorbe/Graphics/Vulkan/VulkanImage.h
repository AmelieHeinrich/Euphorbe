#ifndef EUPHORBE_VULKAN_IMAGE_H
#define EUPHORBE_VULKAN_IMAGE_H

#include <Euphorbe/Graphics/Image.h>
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

typedef struct E_VulkanImage E_VulkanImage;
struct E_VulkanImage
{
    VkImage image;
    VkImageView image_view;
    VkFormat format;
    VkExtent3D image_extent;
    VkSampler sampler;
    VkDescriptorSet gui_descriptor_set;

    VmaAllocation allocation;
    VmaAllocationInfo allocation_info;

    E_ImageFormat euphorbe_format;
};

E_VulkanImage* E_Vk_MakeImage(i32 width, i32 height, E_ImageFormat format);
void E_Vk_FreeImage(E_VulkanImage* image);
void E_Vk_ResizeImage(E_VulkanImage* image, i32 width, i32 height);

void E_Vk_DrawImageToGUI(E_VulkanImage* image, i32 width, i32 height);

#endif