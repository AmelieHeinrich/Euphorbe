#include <volk.h>

typedef struct ImGui_ImplVulkanH_FrameSemaphores ImGui_ImplVulkanH_FrameSemaphores;
typedef struct ImGui_ImplVulkanH_Window ImGui_ImplVulkanH_Window;
typedef struct ImGui_ImplVulkanH_Frame ImGui_ImplVulkanH_Frame;
typedef struct ImGui_ImplVulkan_InitInfo ImGui_ImplVulkan_InitInfo;

struct ImGui_ImplVulkanH_Frame;
struct ImGui_ImplVulkanH_Window;
struct ImGui_ImplVulkan_InitInfo
{
    VkInstance Instance;
    VkPhysicalDevice PhysicalDevice;
    VkDevice Device;
    uint32_t QueueFamily;
    VkQueue Queue;
    VkPipelineCache PipelineCache;
    VkDescriptorPool DescriptorPool;
    uint32_t Subpass;
    uint32_t MinImageCount;
    uint32_t ImageCount;
    VkSampleCountFlagBits MSAASamples;
    const VkAllocationCallbacks* Allocator;
    void (*CheckVkResultFn)(VkResult err);
};
struct ImGui_ImplVulkanH_Frame
{
    VkCommandPool CommandPool;
    VkCommandBuffer CommandBuffer;
    VkFence Fence;
    VkImage Backbuffer;
    VkImageView BackbufferView;
    VkFramebuffer Framebuffer;
};
struct ImGui_ImplVulkanH_FrameSemaphores
{
    VkSemaphore ImageAcquiredSemaphore;
    VkSemaphore RenderCompleteSemaphore;
};
struct ImGui_ImplVulkanH_Window
{
    int Width;
    int Height;
    VkSwapchainKHR Swapchain;
    VkSurfaceKHR Surface;
    VkSurfaceFormatKHR SurfaceFormat;
    VkPresentModeKHR PresentMode;
    VkRenderPass RenderPass;
    VkPipeline Pipeline;
    bool ClearEnable;
    VkClearValue ClearValue;
    uint32_t FrameIndex;
    uint32_t ImageCount;
    uint32_t SemaphoreIndex;
    ImGui_ImplVulkanH_Frame* Frames;
    ImGui_ImplVulkanH_FrameSemaphores* FrameSemaphores;
};
CIMGUI_API bool ImGui_ImplWin32_Init(void* hwnd);
CIMGUI_API void ImGui_ImplWin32_Shutdown();
CIMGUI_API void ImGui_ImplWin32_NewFrame();
CIMGUI_API void ImGui_ImplWin32_EnableDpiAwareness();
CIMGUI_API float ImGui_ImplWin32_GetDpiScaleForHwnd(void* hwnd);
CIMGUI_API float ImGui_ImplWin32_GetDpiScaleForMonitor(void* monitor);
CIMGUI_API void ImGui_ImplWin32_EnableAlphaCompositing(void* hwnd);

CIMGUI_API bool ImGui_ImplVulkan_Init(ImGui_ImplVulkan_InitInfo* info,VkRenderPass render_pass);
CIMGUI_API void ImGui_ImplVulkan_Shutdown();
CIMGUI_API void ImGui_ImplVulkan_NewFrame();
CIMGUI_API void ImGui_ImplVulkan_RenderDrawData(ImDrawData* draw_data, VkCommandBuffer command_buffer, VkPipeline pipeline);
CIMGUI_API bool ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer command_buffer);
CIMGUI_API void ImGui_ImplVulkan_DestroyFontUploadObjects();
CIMGUI_API void ImGui_ImplVulkan_SetMinImageCount(uint32_t min_image_count);
CIMGUI_API void* ImGui_ImplVulkan_AddTexture(VkImageView image_view, VkImageLayout image_layout, VkSampler sampler, VkDescriptorSet descriptor_set);
CIMGUI_API VkDescriptorSetLayout ImGui_ImplVulkan_GetDescriptorSetLayout();

CIMGUI_API void ImGui_ImplVulkanH_CreateOrResizeWindow(VkInstance instance,VkPhysicalDevice physical_device,VkDevice device,ImGui_ImplVulkanH_Window* wnd,uint32_t queue_family,const VkAllocationCallbacks* allocator,int w,int h,uint32_t min_image_count);
CIMGUI_API void ImGui_ImplVulkanH_DestroyWindow(VkInstance instance,VkDevice device,ImGui_ImplVulkanH_Window* wnd,const VkAllocationCallbacks* allocator);
CIMGUI_API VkSurfaceFormatKHR ImGui_ImplVulkanH_SelectSurfaceFormat(VkPhysicalDevice physical_device,VkSurfaceKHR surface,const VkFormat* request_formats,int request_formats_count,VkColorSpaceKHR request_color_space);
CIMGUI_API VkPresentModeKHR ImGui_ImplVulkanH_SelectPresentMode(VkPhysicalDevice physical_device,VkSurfaceKHR surface,const VkPresentModeKHR* request_modes,int request_modes_count);
CIMGUI_API int ImGui_ImplVulkanH_GetMinImageCountFromPresentMode(VkPresentModeKHR present_mode);
