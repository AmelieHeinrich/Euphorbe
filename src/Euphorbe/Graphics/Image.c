#include "Image.h"

#ifdef EUPHORBE_WINDOWS
    #include "Vulkan/VulkanImage.h" 
    #include "Vulkan/VulkanRenderer.h"
#endif

E_Image* E_MakeImage(i32 width, i32 height, E_ImageFormat format)
{
    E_Image* image = malloc(sizeof(E_Image));

    image->format = format;
    image->width = width;
    image->height = height;

#ifdef EUPHORBE_WINDOWS
    image->rhi_handle = E_Vk_MakeImage(width, height, format);
#endif

    return image;
}

void E_FreeImage(E_Image* image)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_FreeImage(image->rhi_handle);
#endif

    free(image->rhi_handle);
    free(image);
}

void E_ImageTransitionLayout(E_Image* image, E_ImageAccess srcAccess, E_ImageAccess dstAccess, E_ImageLayout old, E_ImageLayout new, E_ImagePipelineStage srcStage, E_ImagePipelineStage dstStage)
{
#ifdef EUPHORBE_WINDOWS
    E_VulkanImage* vk_handle = (E_VulkanImage*)image->rhi_handle;

    VkImageSubresourceRange range = {0};
    range.baseMipLevel = 0;
    range.levelCount = VK_REMAINING_MIP_LEVELS;
    range.baseArrayLayer = 0;
    range.layerCount = VK_REMAINING_ARRAY_LAYERS;

    if (image->format == E_ImageFormatRGBA8 || image->format == E_ImageFormatRGBA16)
        range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    else
        range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    E_Vk_Image_Memory_Barrier(rhi.command.command_buffers[rhi.sync.image_index], vk_handle->image, srcAccess, dstAccess, old, new, srcStage, dstStage, range);
#endif
}

void E_ImageResize(E_Image* image, i32 width, i32 height)
{
    image->width = width;
    image->height = height;

#ifdef EUPHORBE_WINDOWS
    E_Vk_ResizeImage(image->rhi_handle, width, height);
#endif
}

void E_ImageDrawToGUI(E_Image* image, i32 width, i32 height)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_DrawImageToGUI(image->rhi_handle, width, height);
#endif
}
