#include "VulkanImage.h"

#include "VulkanRenderer.h"

VkFormat EuphorbeToVulkan(E_ImageFormat format)
{
    switch (format)
    {
    case E_ImageFormatD32_Float:
        return VK_FORMAT_D32_SFLOAT;
    case E_ImageFormatRGBA8:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case E_ImageFormatRGBA16:
        return VK_FORMAT_R16G16B16_SFLOAT;
    }

    return 0;
}

VkImageUsageFlags EuphorbeFormatToVulkanUsage(E_ImageFormat format)
{
    switch (format)
    {
    case E_ImageFormatD32_Float:
        return VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    case E_ImageFormatRGBA8:
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    case E_ImageFormatRGBA16:
        return VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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
        result->format = EuphorbeToVulkan(format);
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

        return result;
    }

    return NULL;
}

void E_Vk_FreeImage(E_VulkanImage* image)
{
    vkDestroyImageView(rhi.device.handle, image->image_view, NULL);
    vmaDestroyImage(rhi.allocator, image->image, image->allocation);
}

void E_Vk_ResizeImage(E_VulkanImage* image, i32 width, i32 height)
{
    E_Vk_FreeImage(image);

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
