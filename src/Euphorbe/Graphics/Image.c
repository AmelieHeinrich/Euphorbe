#include "Image.h"

#ifdef EUPHORBE_WINDOWS
    #include "Vulkan/VulkanImage.h" 
    #include "Vulkan/VulkanRenderer.h"
#endif

E_Image* E_MakeImage(i32 width, i32 height, E_ImageFormat format, E_ImageUsage usage)
{
    E_Image* image = malloc(sizeof(E_Image));

    image->format = format;
    image->width = width;
    image->height = height;
    image->usage = usage;

#ifdef EUPHORBE_WINDOWS
    image->rhi_handle = E_Vk_MakeImage(width, height, format, usage);
#endif

    return image;
}

E_Image* E_MakeImageFromFile(const char* path)
{
    E_Image* image = malloc(sizeof(E_Image));
    image->format = E_ImageFormatRGBA8;

#ifdef EUPHORBE_WINDOWS
    image->rhi_handle = E_Vk_MakeImageFromFile(path);

    E_VulkanImage* vulkan_image = (E_VulkanImage*)image->rhi_handle;

    image->width = vulkan_image->image_extent.width;
    image->height = vulkan_image->image_extent.height;
#endif

    return image;
}

E_Image* E_MakeHDRImageFromFile(const char* path)
{
    E_Image* image = malloc(sizeof(E_Image));
    image->format = E_ImageFormatRGBA32;

#ifdef EUPHORBE_WINDOWS
    image->rhi_handle = E_Vk_MakeHDRImageFromFile(path);

    E_VulkanImage* vulkan_image = (E_VulkanImage*)image->rhi_handle;

    image->width = vulkan_image->image_extent.width;
    image->height = vulkan_image->image_extent.height;
#endif

    return image;
}

E_Image* E_MakeCubeMap(i32 width, i32 height, E_ImageFormat format, E_ImageUsage usage)
{
    E_Image* image = malloc(sizeof(E_Image));

#ifdef EUPHORBE_WINDOWS
    image->rhi_handle = E_Vk_MakeCubeMap(width, height, format, usage);
#endif
    return image;
}

void E_FreeImage(E_Image* image)
{
#ifdef EUPHORBE_WINDOWS
    E_Vk_FreeImage(image->rhi_handle);
#endif

    free(image);
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
