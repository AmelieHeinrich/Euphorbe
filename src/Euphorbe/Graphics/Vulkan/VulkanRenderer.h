#ifndef EUPHORBE_VULKAN_RENDERER_H
#define EUPHORBE_VULKAN_RENDERER_H

#pragma warning(disable: 28251)
#include <Euphorbe/Graphics/Renderer.h>
#include <Euphorbe/Graphics/Vulkan/VulkanMaterial.h>
#include <Euphorbe/Graphics/Vulkan/VulkanBuffer.h>
#include <Euphorbe/Graphics/Vulkan/VulkanCommandBuffer.h>
#include <volk.h>

#include <vk_mem_alloc.h>

typedef struct E_Vk_Data E_Vk_Data;
struct E_Vk_Data
{
    E_Window* window;

    // Instance data
    struct {
        VkInstance handle;
        char* layers[64];
        char* extensions[64];
        i32 layer_count;
        i32 extension_count;
        VkDebugUtilsMessengerEXT debug_messenger;
    } instance;

    // Window surface
    VkSurfaceKHR surface;

    // Physical Device Data
    struct {
        VkPhysicalDevice handle;
        u32 graphics_family;
        u32 compute_family;
        VkPhysicalDeviceProperties handle_props;
        VkPhysicalDeviceFeatures2 features;
    } physical_device;

    // Device data
    struct {
        VkDevice handle;
        VkQueue graphics_queue;
        VkQueue compute_queue;
        char* extensions[64];
        i32 extension_count;
    } device;

    // Swapchain data
    struct {
        VkSwapchainKHR handle;
        VkImage* images;
        VkImageView image_views[FRAMES_IN_FLIGHT];
        VkFormat image_format;
        VkExtent2D extent;
        E_Image* euphorbe_images[FRAMES_IN_FLIGHT];
    } swapchain;

    // Sync data
    struct {
        VkFence fences[FRAMES_IN_FLIGHT];
        VkSemaphore image_available_semaphore;
        VkSemaphore image_rendered_semaphore;
        i32 image_index;
    } sync;

    // Command data
    struct {
        VkCommandPool graphics_command_pool;
        E_CommandBuffer** swapchain_command_buffers;

        VkCommandPool compute_command_pool;
        VkFence compute_fence;

        VkCommandPool upload_command_pool;
        VkFence upload_fence;
    } command;

    // ImGui
    struct {
        VkDescriptorPool descriptor_pool;
        VkRenderPass render_pass;
        VkFramebuffer swapchain_framebuffers[FRAMES_IN_FLIGHT];
    } imgui;

    // Other
    VkDescriptorPool global_descriptor_pool;

    // VMA
    VmaAllocator allocator;
};

extern E_Vk_Data rhi;
extern E_RendererInitSettings rhi_settings;

void E_Vk_RendererInit(E_Window* window, E_RendererInitSettings settings);
void E_Vk_RendererShutdown();

void E_Vk_Begin();
void E_Vk_End();
void E_Vk_DeviceWait();
void E_Vk_DrawMemoryUsageGUI();
void E_Vk_DrawGraphicsCardInfo();

void E_Vk_Resize(i32 width, i32 height);

void E_Vk_BeginGUI();
void E_Vk_EndGUI();

E_Image* E_Vk_GetSwapchainImage();
u32 E_Vk_GetSwapchainImageIndex();
E_CommandBuffer* E_Vk_GetSwapchainCommandBuffer();

// Most useful function for dynamic rendering
void E_Vk_Image_Memory_Barrier(VkCommandBuffer command_buffer, 
                               VkImage image,
                               VkAccessFlags src_access_mask,
                               VkAccessFlags dst_access_mask,
                               VkImageLayout old_layout,
                               VkImageLayout new_layout,
                               VkPipelineStageFlags src_stage_mask,
                               VkPipelineStageFlags dst_stage_mask,
                               VkImageSubresourceRange subresource_range);

#endif