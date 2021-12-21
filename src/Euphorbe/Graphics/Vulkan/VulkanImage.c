#include "VulkanImage.h"

#include "VulkanRenderer.h"

#include <cimgui_impl.h>

VkImageUsageFlags EuphorbeFormatToVulkanUsage(E_ImageFormat format)
{
    switch (format)
    {
    case E_ImageFormatD32_Float:
        return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    case E_ImageFormatRGBA8:
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    case E_ImageFormatRGBA16:
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    return 0;
}

VkImageAspectFlags EuphorbeFormatToVulkanAspect(E_ImageFormat format)
{
    switch (format)
    {
    case E_ImageFormatD32_Float:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case E_ImageFormatRGBA8:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    case E_ImageFormatRGBA16:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    }

    return 0;
}

E_VulkanImage* E_Vk_MakeImage(i32 width, i32 height, E_ImageFormat format)
{
    E_VulkanImage* result = malloc(sizeof(E_VulkanImage));

    // Only used to make the C6011 warning go away
    if (result)
    {
        result->format = (VkFormat)format;
        result->euphorbe_format = format;

        VkImageCreateInfo image_create_info = { 0 };
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_create_info.imageType = VK_IMAGE_TYPE_2D;
        image_create_info.extent.width = width;
        image_create_info.extent.height = height;
        image_create_info.format = result->format;
        image_create_info.extent.depth = 1;
        image_create_info.mipLevels = 1;
        image_create_info.arrayLayers = 1;
        image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
        image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_create_info.usage = EuphorbeFormatToVulkanUsage(format);
        image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
        image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        result->image_extent = image_create_info.extent;

        VmaAllocationCreateInfo allocation = { 0 };
        allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VkResult res = vmaCreateImage(rhi.allocator, &image_create_info, &allocation, &result->image, &result->allocation, &result->allocation_info);
        assert(res == VK_SUCCESS);

        VkImageViewCreateInfo view_info = { 0 };
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = result->image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = result->format;
        view_info.subresourceRange.aspectMask = EuphorbeFormatToVulkanAspect(format);
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = 1;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;
        view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        res = vkCreateImageView(rhi.device.handle, &view_info, NULL, &result->image_view);
        assert(res == VK_SUCCESS);

        VkSamplerCreateInfo sampler_info = {0};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;
        sampler_info.maxAnisotropy = rhi.physical_device.handle_props.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

        res = vkCreateSampler(rhi.device.handle, &sampler_info, NULL, &result->sampler);
        assert(res == VK_SUCCESS);

        VkDescriptorSetLayout set_layout = ImGui_ImplVulkan_GetDescriptorSetLayout();

        VkDescriptorSetAllocateInfo alloc_info = { 0 };
        alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &set_layout;
        alloc_info.descriptorPool = rhi.imgui.descriptor_pool;

        res = vkAllocateDescriptorSets(rhi.device.handle, &alloc_info, &result->gui_descriptor_set);
        assert(res == VK_SUCCESS);

        return result;
    }

    return NULL;
}

void E_Vk_FreeImage(E_VulkanImage* image)
{
    vkFreeDescriptorSets(rhi.device.handle, rhi.imgui.descriptor_pool, 1, &image->gui_descriptor_set);
    vkDestroySampler(rhi.device.handle, image->sampler, NULL);
    vkDestroyImageView(rhi.device.handle, image->image_view, NULL);
    vmaDestroyImage(rhi.allocator, image->image, image->allocation);
}

void E_Vk_ResizeImage(E_VulkanImage* image, i32 width, i32 height)
{
    vkDestroyImageView(rhi.device.handle, image->image_view, NULL);
    vmaDestroyImage(rhi.allocator, image->image, image->allocation);

    VkImageCreateInfo image_create_info = { 0 };
    image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_create_info.imageType = VK_IMAGE_TYPE_2D;
    image_create_info.extent.width = width;
    image_create_info.extent.height = height;
    image_create_info.format = image->format;
    image_create_info.extent.depth = 1;
    image_create_info.mipLevels = 1;
    image_create_info.arrayLayers = 1;
    image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_create_info.usage = EuphorbeFormatToVulkanUsage(image->euphorbe_format);
    image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    image->image_extent = image_create_info.extent;

    VmaAllocationCreateInfo allocation = { 0 };
    allocation.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    VkResult res = vmaCreateImage(rhi.allocator, &image_create_info, &allocation, &image->image, &image->allocation, &image->allocation_info);
    assert(res == VK_SUCCESS);

    VkImageViewCreateInfo view_info = { 0 };
    view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    view_info.image = image->image;
    view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    view_info.format = image->format;
    view_info.subresourceRange.aspectMask = EuphorbeFormatToVulkanAspect(image->euphorbe_format);
    view_info.subresourceRange.baseMipLevel = 0;
    view_info.subresourceRange.levelCount = 1;
    view_info.subresourceRange.baseArrayLayer = 0;
    view_info.subresourceRange.layerCount = 1;
    view_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    view_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    res = vkCreateImageView(rhi.device.handle, &view_info, NULL, &image->image_view);
    assert(res == VK_SUCCESS);
}

void E_Vk_DrawImageToGUI(E_VulkanImage* image, i32 width, i32 height)
{
    ImVec2 size = { (f32)width, (f32)height };

    ImVec2 uv0 = { 0, 0 };
    ImVec2 uv1 = { 1, 1 };
    ImVec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
    ImVec4 border = { 0.0f, 0.0f, 0.0f, 0.0f };

    ImGui_ImplVulkan_AddTexture(image->image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, image->sampler, image->gui_descriptor_set);
    igImage(image->gui_descriptor_set, size, uv0, uv1, color, border);
}
