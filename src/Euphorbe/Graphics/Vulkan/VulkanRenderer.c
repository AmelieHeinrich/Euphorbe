#include "VulkanRenderer.h"

#pragma warning(disable: 6385)

#include <Euphorbe/Core/Log.h>

#ifdef EUPHORBE_WINDOWS
    #include <vulkan/vulkan_win32.h>
    #include <Euphorbe/Platform/Windows/WindowsWindow.h>
#endif

#include "VulkanImage.h"

E_Vk_Data rhi;

static VkBool32 E_CheckLayers(u32 check_count, char **check_names,
                                  u32 layer_count,
                                  VkLayerProperties *layers) {
    for (u32 i = 0; i < check_count; i++) {
        VkBool32 found = 0;
        for (u32 j = 0; j < layer_count; j++) {
            E_LogInfo("Found instance layer: %s", layers[j].layerName);
            if (strcmp(check_names[i], layers[j].layerName) == 0) {
                found = 1;
                break;
            }
        }
    }
    return 1;
}

void E_Vk_MakeInstance()
{
    rhi.instance.layer_count = 0;
    rhi.instance.extension_count = 0;

    u32 instance_extension_count = 0;
    u32 instance_layer_count = 0;
    u32 validation_layer_count = 0;
    char** instance_validation_layers = NULL;

    char* instance_validation_layers_alt1[] = {
        "VK_LAYER_KHRONOS_validation"
    };

    b32 validation_found;
    VkResult result = vkEnumerateInstanceLayerProperties(&instance_layer_count, NULL);
    assert(result == VK_SUCCESS);

    instance_validation_layers = instance_validation_layers_alt1;

    if (instance_layer_count > 0) {
        VkLayerProperties* instance_layers = malloc(sizeof (VkLayerProperties) * instance_layer_count);
        result = vkEnumerateInstanceLayerProperties(&instance_layer_count, instance_layers);
        assert(result == VK_SUCCESS);

        validation_found = E_CheckLayers(ARRAY_SIZE(instance_validation_layers_alt1), instance_validation_layers, instance_layer_count, instance_layers);

        if (validation_found) {
            rhi.instance.layer_count = 1;
            rhi.instance.layers[0] = "VK_LAYER_KHRONOS_validation";
            validation_layer_count = 1;
        }

        free(instance_layers);
    }

     memset(rhi.instance.extensions, 0, sizeof(rhi.instance.extensions));
     result = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
     assert(result == VK_SUCCESS);

    if (instance_extension_count > 0) {
        VkExtensionProperties *instance_extensions = malloc(sizeof(VkExtensionProperties) * instance_extension_count);
        if (instance_extensions)
        {
            result = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, instance_extensions);
            assert(result == VK_SUCCESS);

            for (u32 i = 0; i < instance_extension_count; i++) {
                E_LogInfo("Found instance extension: %s", instance_extensions[i].extensionName);

                if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                    rhi.instance.extensions[rhi.instance.extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
                }

#ifdef EUPHORBE_WINDOWS
                if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                    rhi.instance.extensions[rhi.instance.extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
                }
#endif

                assert(rhi.instance.extension_count < 64);
            }

            free(instance_extensions);
        }
    }

    VkApplicationInfo app_info = {0};
    app_info.pNext = NULL;
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = rhi.window->title;
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.pEngineName = "Euphorbe";
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_API_VERSION_1_2;

    VkInstanceCreateInfo create_info = {0};
    create_info.pNext = NULL;
    create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    create_info.pApplicationInfo = &app_info;

#ifdef _DEBUG
    create_info.enabledLayerCount = rhi.instance.layer_count;
    create_info.ppEnabledLayerNames = (const char *const *)rhi.instance.layers;
#else
    create_info.enabledLayerCount = 0; 
    create_info.ppEnabledLayerNames = NULL; 
#endif

    create_info.enabledExtensionCount = rhi.instance.extension_count;
    create_info.ppEnabledExtensionNames = (const char *const *)rhi.instance.extensions;

    result = vkCreateInstance(&create_info, NULL, &rhi.instance.handle);
    assert(result == VK_SUCCESS);

    volkLoadInstance(rhi.instance.handle);
}

void E_Vk_MakeSurface()
{
#ifdef EUPHORBE_WINDOWS
    E_WindowsWindow* window = (E_WindowsWindow*)rhi.window->platform_data;
    HWND hwnd = window->hwnd;
    assert(hwnd);

    VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.hinstance = GetModuleHandle(NULL);
    surface_create_info.hwnd = hwnd;
    surface_create_info.flags = 0;
    surface_create_info.pNext = NULL;

    // Load it myself because Volk is annoying
    PFN_vkCreateWin32SurfaceKHR function_pointer = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(rhi.instance.handle, "vkCreateWin32SurfaceKHR");
    assert(function_pointer);

    VkResult result = function_pointer(rhi.instance.handle, &surface_create_info, NULL, &rhi.surface);
    assert(result == VK_SUCCESS);
#endif
}

void E_Vk_MakePhysicalDevice()
{
    u32 device_count = 0;
    vkEnumeratePhysicalDevices(rhi.instance.handle, &device_count, NULL);
    assert(device_count != 0);

    VkPhysicalDevice* devices = malloc(sizeof(VkPhysicalDevice) * device_count);
    if (devices)
    {
        vkEnumeratePhysicalDevices(rhi.instance.handle, &device_count, devices);
        rhi.physical_device.handle = devices[0];
        free(devices);
    }

    vkGetPhysicalDeviceProperties(rhi.physical_device.handle, &rhi.physical_device.handle_props);

    // Find queue families
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(rhi.physical_device.handle, &queue_family_count, NULL);

    VkQueueFamilyProperties* queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
    if (queue_families)
    {
        vkGetPhysicalDeviceQueueFamilyProperties(rhi.physical_device.handle, &queue_family_count, queue_families);

        for (u32 i = 0; i < queue_family_count; i++)
        {
            if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            {
                rhi.physical_device.graphics_family = i;
                break;
            }
        }

        free(queue_families);
    }
}

void E_Vk_MakeDevice()
{
    rhi.device.extension_count = 0;

    f32 queuePriority = 1.0f;

    VkDeviceQueueCreateInfo graphics_queue_create_info = { 0 };
    graphics_queue_create_info.flags = 0;
    graphics_queue_create_info.pNext = NULL;
    graphics_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    graphics_queue_create_info.queueFamilyIndex = rhi.physical_device.graphics_family;
    graphics_queue_create_info.queueCount = 1;
    graphics_queue_create_info.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures features = {0};
    features.samplerAnisotropy = 1;
    features.fillModeNonSolid = 1;

    u32 extension_count = 0;
    vkEnumerateDeviceExtensionProperties(rhi.physical_device.handle, NULL, &extension_count, NULL);
    VkExtensionProperties* properties = malloc(sizeof(VkExtensionProperties) * extension_count);
    if (properties)
    {
        vkEnumerateDeviceExtensionProperties(rhi.physical_device.handle, NULL, &extension_count, properties);

        for (u32 i = 0; i < extension_count; i++)
        {
            E_LogInfo("Found device extension: %s", properties[i].extensionName);

            if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;
            }
        }
        assert(rhi.device.extension_count == 2);

        free(properties);
    }

    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = 1;
    create_info.pQueueCreateInfos = &graphics_queue_create_info;
    create_info.pEnabledFeatures = &features;
    create_info.enabledExtensionCount = rhi.device.extension_count;
    create_info.ppEnabledExtensionNames = (const char* const*)rhi.device.extensions;

#ifdef _DEBUG
    create_info.enabledLayerCount = rhi.instance.layer_count;
    create_info.ppEnabledLayerNames = (const char* const*)rhi.instance.layers;
#else
    create_info.enabledLayerCount = 0;
    create_info.ppEnabledLayerNames = NULL;
#endif

    VkResult result = vkCreateDevice(rhi.physical_device.handle, &create_info, NULL, &rhi.device.handle);
    assert(result == VK_SUCCESS);

    volkLoadDevice(rhi.device.handle);
    vkGetDeviceQueue(rhi.device.handle, rhi.physical_device.graphics_family, 0, &rhi.device.graphics_queue);
}

void E_Vk_MakeSwapchain()
{
    rhi.swapchain.extent.width = rhi.window->width;
    rhi.swapchain.extent.height = rhi.window->height;
    assert(rhi.swapchain.extent.width != 0 && rhi.swapchain.extent.height != 0);

    u32 queue_family_indices[] = { rhi.physical_device.graphics_family };

    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(rhi.physical_device.handle, rhi.surface, &capabilities);

    u32 format_count;
    vkGetPhysicalDeviceSurfaceFormatsKHR(rhi.physical_device.handle, rhi.surface, &format_count, NULL);
    VkSurfaceFormatKHR* formats = malloc(sizeof(VkSurfaceFormatKHR) * format_count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(rhi.physical_device.handle, rhi.surface, &format_count, formats);

    VkSwapchainCreateInfoKHR create_info = { 0 };
    create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    create_info.surface = rhi.surface;
    create_info.minImageCount = FRAMES_IN_FLIGHT;
    create_info.imageExtent = rhi.swapchain.extent;
    create_info.imageArrayLayers = 1;
    create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    create_info.queueFamilyIndexCount = 1;
    create_info.pQueueFamilyIndices = queue_family_indices;
    create_info.preTransform = capabilities.currentTransform;
    create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    create_info.clipped = VK_TRUE;
    create_info.oldSwapchain = VK_NULL_HANDLE;
    if (formats)
    {
        create_info.imageFormat = formats[0].format;
        create_info.imageColorSpace = formats[0].colorSpace;
        free(formats);
    }

    rhi.swapchain.image_format = create_info.imageFormat;

    VkResult result = vkCreateSwapchainKHR(rhi.device.handle, &create_info, NULL, &rhi.swapchain.handle);
    assert(result == VK_SUCCESS);

    i32 image_count = 0;
    vkGetSwapchainImagesKHR(rhi.device.handle, rhi.swapchain.handle, &image_count, NULL);
    rhi.swapchain.images = malloc(sizeof(VkImage) * image_count);
    if (rhi.swapchain.images)
    {
        vkGetSwapchainImagesKHR(rhi.device.handle, rhi.swapchain.handle, &image_count, rhi.swapchain.images);

        for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        {
            VkImageViewCreateInfo iv_create_info = { 0 };
            iv_create_info.flags = 0;
            iv_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            iv_create_info.image = rhi.swapchain.images[i];
            iv_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            iv_create_info.format = rhi.swapchain.image_format;
            iv_create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            iv_create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            iv_create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            iv_create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            iv_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            iv_create_info.subresourceRange.baseMipLevel = 0;
            iv_create_info.subresourceRange.levelCount = 1;
            iv_create_info.subresourceRange.baseArrayLayer = 0;
            iv_create_info.subresourceRange.layerCount = 1;

            result = vkCreateImageView(rhi.device.handle, &iv_create_info, NULL, &rhi.swapchain.image_views[i]);
            assert(result == VK_SUCCESS);

            rhi.swapchain.euphorbe_images[i] = malloc(sizeof(E_Image));
            rhi.swapchain.euphorbe_images[i]->rhi_handle = malloc(sizeof(E_VulkanImage));
            rhi.swapchain.euphorbe_images[i]->format = E_ImageFormatRGBA8;
            rhi.swapchain.euphorbe_images[i]->width = rhi.window->width;
            rhi.swapchain.euphorbe_images[i]->height = rhi.window->height;

            E_VulkanImage* image_handle = (E_VulkanImage*)rhi.swapchain.euphorbe_images[i]->rhi_handle;
            image_handle->format = create_info.imageFormat;
            image_handle->image = rhi.swapchain.images[i];
            image_handle->image_view = rhi.swapchain.image_views[i];
        }
    }
}

void E_Vk_MakeSync()
{
    VkResult result;

    for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++) {
        VkFenceCreateInfo fence_info = { 0 };
        fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        result = vkCreateFence(rhi.device.handle, &fence_info, NULL, &rhi.sync.fences[i]);
        assert(result == VK_SUCCESS);
    }

    VkSemaphoreCreateInfo semaphore_info = { 0 };
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    result = vkCreateSemaphore(rhi.device.handle, &semaphore_info, NULL, &rhi.sync.image_available_semaphore);
    assert(result == VK_SUCCESS);
    result = vkCreateSemaphore(rhi.device.handle, &semaphore_info, NULL, &rhi.sync.image_rendered_semaphore);
    assert(result == VK_SUCCESS);
}

void E_Vk_MakeCommand()
{
    VkCommandPoolCreateInfo pool_info = { 0 };
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = rhi.physical_device.graphics_family;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult result = vkCreateCommandPool(rhi.device.handle, &pool_info, NULL, &rhi.command.graphics_command_pool);
    assert(result == VK_SUCCESS);

    rhi.command.command_buffers = malloc(sizeof(VkCommandBuffer) * FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo alloc_info = { 0 };
    alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    alloc_info.commandPool = rhi.command.graphics_command_pool;
    alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    alloc_info.commandBufferCount = FRAMES_IN_FLIGHT;

    result = vkAllocateCommandBuffers(rhi.device.handle, &alloc_info, rhi.command.command_buffers);
    assert(result == VK_SUCCESS);
}

void E_Vk_MakeAllocator()
{
    VmaAllocatorCreateInfo allocator_info = { 0 };
    allocator_info.device = rhi.device.handle;
    allocator_info.instance = rhi.instance.handle;
    allocator_info.physicalDevice = rhi.physical_device.handle;
    allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
    VmaVulkanFunctions vulkanFunctions = {
            vkGetPhysicalDeviceProperties,
            vkGetPhysicalDeviceMemoryProperties,
            vkAllocateMemory,
            vkFreeMemory,
            vkMapMemory,
            vkUnmapMemory,
            vkFlushMappedMemoryRanges,
            vkInvalidateMappedMemoryRanges,
            vkBindBufferMemory,
            vkBindImageMemory,
            vkGetBufferMemoryRequirements,
            vkGetImageMemoryRequirements,
            vkCreateBuffer,
            vkDestroyBuffer,
            vkCreateImage,
            vkDestroyImage,
            vkCmdCopyBuffer,
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
                vkGetBufferMemoryRequirements2,
                vkGetImageMemoryRequirements2,
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
                vkBindBufferMemory2,
                vkBindImageMemory2,
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000
                vkGetPhysicalDeviceMemoryProperties2,
#endif
    };

    allocator_info.pVulkanFunctions = &vulkanFunctions;

    VkResult result = vmaCreateAllocator(&allocator_info, &rhi.allocator);
    assert(result == VK_SUCCESS);
}

// Most useful function for dynamic rendering
void E_Vk_Image_Memory_Barrier(VkCommandBuffer command_buffer, 
                               VkImage image,
                               VkAccessFlags src_access_mask,
                               VkAccessFlags dst_access_mask,
                               VkImageLayout old_layout,
                               VkImageLayout new_layout,
                               VkPipelineStageFlags src_stage_mask,
                               VkPipelineStageFlags dst_stage_mask,
                               VkImageSubresourceRange subresource_range)
{
    VkImageMemoryBarrier barrier = {0};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.srcAccessMask = src_access_mask;
    barrier.dstAccessMask = dst_access_mask;
    barrier.oldLayout = old_layout;
    barrier.newLayout = new_layout;
    barrier.image = image;
    barrier.subresourceRange = subresource_range;

    vkCmdPipelineBarrier(
        command_buffer,
        src_stage_mask,
        dst_stage_mask,
        0,
        0, NULL,
        0, NULL,
        1, &barrier);
}

void E_Vk_RendererInit(E_Window* window)
{
    rhi.window = window;
    assert(rhi.window);
    
    VkResult result = volkInitialize();
    assert(result == VK_SUCCESS);

    E_Vk_MakeInstance();
    E_Vk_MakeSurface();
    E_Vk_MakePhysicalDevice();
    E_Vk_MakeDevice();
    E_Vk_MakeSwapchain();
    E_Vk_MakeSync();
    E_Vk_MakeCommand();
    E_Vk_MakeAllocator();
}

void E_Vk_RendererShutdown()
{
    vkDestroySemaphore(rhi.device.handle, rhi.sync.image_available_semaphore, NULL);
    vkDestroySemaphore(rhi.device.handle, rhi.sync.image_rendered_semaphore, NULL);
    vkDestroyCommandPool(rhi.device.handle, rhi.command.graphics_command_pool, NULL);

    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        free(rhi.swapchain.euphorbe_images[i]->rhi_handle);
        free(rhi.swapchain.euphorbe_images[i]);
        vkDestroyFence(rhi.device.handle, rhi.sync.fences[i], NULL);
        vkDestroyImageView(rhi.device.handle, rhi.swapchain.image_views[i], NULL);
    }

    vkDestroySwapchainKHR(rhi.device.handle, rhi.swapchain.handle, NULL);
    free(rhi.swapchain.images);
    vkDestroyDevice(rhi.device.handle, NULL);
    vkDestroySurfaceKHR(rhi.instance.handle, rhi.surface, NULL);
    vkDestroyInstance(rhi.instance.handle, NULL);
}

void E_Vk_Begin()
{
    vkAcquireNextImageKHR(rhi.device.handle, rhi.swapchain.handle, UINT32_MAX, rhi.sync.image_available_semaphore, VK_NULL_HANDLE, &rhi.sync.image_index);

    vkWaitForFences(rhi.device.handle, 1, &rhi.sync.fences[rhi.sync.image_index], VK_TRUE, UINT32_MAX);
    vkResetFences(rhi.device.handle, 1, &rhi.sync.fences[rhi.sync.image_index]);
    vkResetCommandBuffer(rhi.command.command_buffers[rhi.sync.image_index], 0);

    VkCommandBuffer commandBuffer = rhi.command.command_buffers[rhi.sync.image_index];

    VkCommandBufferBeginInfo begin_info = { 0 };
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.pInheritanceInfo = NULL;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    VkResult res = vkBeginCommandBuffer(commandBuffer, &begin_info);
    assert(res == VK_SUCCESS);

    VkViewport viewport = { 0 };
    viewport.width = (f32)rhi.window->width;
    viewport.height = (f32)rhi.window->height;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = { 0 };
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent = rhi.swapchain.extent;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void E_Vk_End()
{
    VkCommandBuffer command_buffer = rhi.command.command_buffers[rhi.sync.image_index];

    VkResult result = vkEndCommandBuffer(command_buffer);
    assert(result == VK_SUCCESS);

    // Submit

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = { rhi.sync.image_available_semaphore };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &command_buffer;

    VkSemaphore signal_semaphores[] = { rhi.sync.image_rendered_semaphore };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(rhi.device.handle, 1, &rhi.sync.fences[rhi.sync.image_index]);

    result = vkQueueSubmit(rhi.device.graphics_queue, 1, &submit_info, rhi.sync.fences[rhi.sync.image_index]);
    assert(result == VK_SUCCESS);

    VkPresentInfoKHR present_info = { 0 };
    present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present_info.waitSemaphoreCount = 1;
    present_info.pWaitSemaphores = signal_semaphores;

    VkSwapchainKHR swap_chains[] = { rhi.swapchain.handle };
    present_info.swapchainCount = 1;
    present_info.pSwapchains = swap_chains;
    present_info.pImageIndices = &rhi.sync.image_index;

    vkQueuePresentKHR(rhi.device.graphics_queue, &present_info);
}

void E_Vk_DeviceWait()
{
    vkDeviceWaitIdle(rhi.device.handle);
}

#pragma optimize("",off)
void E_Vk_RendererStartRender(E_ImageAttachment* attachments, i32 attachment_count, i32 has_depth)
{
    u32 color_iterator = has_depth ? attachment_count - 1 : attachment_count;

    VkRect2D render_area = {0};
    render_area.extent.width = rhi.window->width;
    render_area.extent.height = rhi.window->height;
    render_area.offset.x = 0;
    render_area.offset.y = 0;

    VkRenderingInfoKHR rendering_info = {0};
    rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
    rendering_info.renderArea = render_area;
    rendering_info.colorAttachmentCount = color_iterator;
    rendering_info.layerCount = 1;

    // Max attachment count is 64
    VkRenderingAttachmentInfoKHR color_attachments[64] = { 0 };

    for (u32 i = 0; i < color_iterator; i++)
    {
        E_VulkanImage* vk_image = (E_VulkanImage*)attachments[i].image->rhi_handle;

        VkClearValue clear_value = {0};
        clear_value.color.float32[0] = attachments[i].clear_value.r;
        clear_value.color.float32[1] = attachments[i].clear_value.g;
        clear_value.color.float32[2] = attachments[i].clear_value.b;
        clear_value.color.float32[3] = attachments[i].clear_value.a;

        VkRenderingAttachmentInfoKHR color_attachment_info = {0};
        color_attachment_info.sType                        = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
		color_attachment_info.imageView                    = vk_image->image_view;
		color_attachment_info.imageLayout                  = attachments[i].layout;
		color_attachment_info.resolveMode                  = VK_RESOLVE_MODE_NONE;
		color_attachment_info.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment_info.storeOp                      = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment_info.clearValue                   = clear_value;

        color_attachments[i] = color_attachment_info;
    }

    if (has_depth)
    {
        E_VulkanImage* vk_image = (E_VulkanImage*)attachments[color_iterator].image->rhi_handle;

        VkClearValue depth_clear_value = {0};
        depth_clear_value.depthStencil.depth = attachments[color_iterator].clear_value.depth;
        depth_clear_value.depthStencil.stencil = attachments[color_iterator].clear_value.stencil;

        VkRenderingAttachmentInfoKHR depth_attachment = {0};
        depth_attachment.sType                        = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        depth_attachment.imageView                    = vk_image->image_view;
		depth_attachment.imageLayout                  = attachments[color_iterator].layout;
		depth_attachment.resolveMode                  = VK_RESOLVE_MODE_NONE;
		depth_attachment.loadOp                       = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp                      = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.clearValue                   = depth_clear_value;

        rendering_info.pStencilAttachment = &depth_attachment;
        rendering_info.pDepthAttachment = &depth_attachment;
    }

    rendering_info.pColorAttachments = color_attachments;

    vkCmdBeginRenderingKHR(rhi.command.command_buffers[rhi.sync.image_index], &rendering_info);
}
#pragma optimize("",on)

void E_Vk_RendererEndRender()
{
    vkCmdEndRenderingKHR(rhi.command.command_buffers[rhi.sync.image_index]);
}

E_Image* E_Vk_GetSwapchainImage()
{
    return rhi.swapchain.euphorbe_images[rhi.sync.image_index];
}

void E_Vk_Resize(i32 width, i32 height)
{
    E_Vk_DeviceWait();

    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        free(rhi.swapchain.euphorbe_images[i]->rhi_handle);
        free(rhi.swapchain.euphorbe_images[i]);
        vkDestroyImageView(rhi.device.handle, rhi.swapchain.image_views[i], NULL);
    }

    vkDestroySwapchainKHR(rhi.device.handle, rhi.swapchain.handle, NULL);
    free(rhi.swapchain.images);

    E_Vk_MakeSwapchain();

    E_LogInfo("Recreated swapchain with new dimensions: {%d, %d}", width, height);
}