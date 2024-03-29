#include "VulkanRenderer.h"

#pragma warning(disable: 6385)

#include <Euphorbe/Core/Log.h>

#ifdef EUPHORBE_WINDOWS
    #include <Euphorbe/Platform/Windows/WindowsWindow.h>
#endif

#include "VulkanImage.h"

#include <cimgui.h>
#include <cimgui_impl.h>

#define CURRENT_CMD_BUF (E_VulkanCommandBuffer*)rhi.command.swapchain_command_buffers[rhi.sync.image_index]->rhi_handle

E_Vk_Data rhi;
E_RendererInitSettings rhi_settings;

static VkBool32 E_CheckLayers(u32 check_count, char **check_names,
                                  u32 layer_count,
                                  VkLayerProperties *layers) {
    for (u32 i = 0; i < check_count; i++) {
        VkBool32 found = 0;
        for (u32 j = 0; j < layer_count; j++) {
            if (rhi_settings.log_found_layers)
                E_LogInfo("Found instance layer: %s", layers[j].layerName);
            if (strcmp(check_names[i], layers[j].layerName) == 0) {
                found = 1;
                break;
            }
        }
    }
    return 1;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL E_Vk_DebugMessenger(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
    if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        printf("%s\n", pCallbackData->pMessage);

    return VK_FALSE;
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
                if (rhi_settings.log_found_layers)
                    E_LogInfo("RENDERER LAYERS: Found instance extension: %s", instance_extensions[i].extensionName);

                if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                    rhi.instance.extensions[rhi.instance.extension_count++] = VK_KHR_SURFACE_EXTENSION_NAME;
                }

                if (!strcmp(VK_EXT_DEBUG_UTILS_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                    rhi.instance.extensions[rhi.instance.extension_count++] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
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
    create_info.enabledExtensionCount = rhi.instance.extension_count;
    create_info.ppEnabledExtensionNames = (const char *const *)rhi.instance.extensions;
    if (rhi_settings.enable_debug)
    {
        create_info.enabledLayerCount = rhi.instance.layer_count;
        create_info.ppEnabledLayerNames = (const char* const*)rhi.instance.layers;
    }
    else
    {
        create_info.enabledLayerCount = 0;
        create_info.ppEnabledLayerNames = NULL;
    }

    result = vkCreateInstance(&create_info, NULL, &rhi.instance.handle);
    assert(result == VK_SUCCESS);

    volkLoadInstance(rhi.instance.handle);

    // Create debug messenger
    VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info = { 0 };
    debug_messenger_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debug_messenger_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debug_messenger_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debug_messenger_info.pfnUserCallback = E_Vk_DebugMessenger;

    result = vkCreateDebugUtilsMessengerEXT(rhi.instance.handle, &debug_messenger_info, NULL, &rhi.instance.debug_messenger);
    assert(result == VK_SUCCESS);

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Instance and debug messenger created");
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

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Surface created");
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

    rhi.physical_device.handle_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    rhi.physical_device.mesh_shader_props.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;

    rhi.physical_device.handle_props.pNext = &rhi.physical_device.mesh_shader_props;
    vkGetPhysicalDeviceProperties2(rhi.physical_device.handle, &rhi.physical_device.handle_props);

    rhi.physical_device.features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    vkGetPhysicalDeviceFeatures2(rhi.physical_device.handle, &rhi.physical_device.features);

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
            }

            if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
            {
                rhi.physical_device.compute_family = i;
            }
        }

        free(queue_families);
    }

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Physical Device found");
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

    VkDeviceQueueCreateInfo compute_queue_create_info = { 0 };
    compute_queue_create_info.flags = 0;
    compute_queue_create_info.pNext = NULL;
    compute_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    compute_queue_create_info.queueFamilyIndex = rhi.physical_device.compute_family;
    compute_queue_create_info.queueCount = 1;
    compute_queue_create_info.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures features = {0};
    features.samplerAnisotropy = 1;
    features.fillModeNonSolid = 1;
    features.geometryShader = 1;
    features.pipelineStatisticsQuery = 1;

    rhi.physical_device.features.features = features;

    VkPhysicalDevice16BitStorageFeatures features16 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES };
    features16.storageBuffer16BitAccess = true;

    VkPhysicalDevice8BitStorageFeaturesKHR features8 = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES_KHR };
    features8.storageBuffer8BitAccess = true;
    features8.uniformAndStorageBuffer8BitAccess = true;
    features8.pNext = &features16;

    VkPhysicalDeviceMeshShaderFeaturesNV mesh_shader_features = { 0 };
    mesh_shader_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV;
    mesh_shader_features.taskShader = VK_TRUE;
    mesh_shader_features.meshShader = VK_TRUE;
    mesh_shader_features.pNext = &features8;

    VkPhysicalDeviceDynamicRenderingFeaturesKHR dynamic_features = { 0 };
    dynamic_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR;
    dynamic_features.dynamicRendering = 1;
    dynamic_features.pNext = &mesh_shader_features;

    rhi.physical_device.features.pNext = &dynamic_features;

    u32 extension_count = 0;
    vkEnumerateDeviceExtensionProperties(rhi.physical_device.handle, NULL, &extension_count, NULL);
    VkExtensionProperties* properties = malloc(sizeof(VkExtensionProperties) * extension_count);
    if (properties)
    {
        vkEnumerateDeviceExtensionProperties(rhi.physical_device.handle, NULL, &extension_count, properties);

        for (u32 i = 0; i < extension_count; i++)
        {
            if (rhi_settings.log_found_layers)
                E_LogInfo("RENDERER LAYERS: Found device extension: %s", properties[i].extensionName);

            if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_SYNCHRONIZATION_2_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME;
            }

            if (!strcmp(VK_NV_MESH_SHADER_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_NV_MESH_SHADER_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_16BIT_STORAGE_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_16BIT_STORAGE_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_8BIT_STORAGE_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_8BIT_STORAGE_EXTENSION_NAME;
            }

            if (!strcmp(VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME, properties[i].extensionName)) {
                rhi.device.extensions[rhi.device.extension_count++] = VK_KHR_SHADER_NON_SEMANTIC_INFO_EXTENSION_NAME;
            }
        }
        assert(rhi.device.extension_count == 8);

        free(properties);
    }

    VkDeviceQueueCreateInfo queue_create_infos[2] = {graphics_queue_create_info, compute_queue_create_info};
    i32 queue_create_info_count = 2;

    VkDeviceCreateInfo create_info = {0};
    create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    create_info.queueCreateInfoCount = 2;
    create_info.pQueueCreateInfos = queue_create_infos;
    create_info.enabledExtensionCount = rhi.device.extension_count;
    create_info.ppEnabledExtensionNames = (const char* const*)rhi.device.extensions;
    create_info.pNext = &rhi.physical_device.features;
    if (rhi_settings.enable_debug)
    {
        create_info.enabledLayerCount = rhi.instance.layer_count;
        create_info.ppEnabledLayerNames = (const char* const*)rhi.instance.layers;
    }
    else
    {
        create_info.enabledLayerCount = 0;
        create_info.ppEnabledLayerNames = NULL;
    }

    VkResult result = vkCreateDevice(rhi.physical_device.handle, &create_info, NULL, &rhi.device.handle);
    assert(result == VK_SUCCESS);

    volkLoadDevice(rhi.device.handle);
    vkGetDeviceQueue(rhi.device.handle, rhi.physical_device.graphics_family, 0, &rhi.device.graphics_queue);
    vkGetDeviceQueue(rhi.device.handle, rhi.physical_device.compute_family, 0, &rhi.device.compute_queue);

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Device created");
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
    create_info.presentMode = rhi_settings.enable_vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_MAILBOX_KHR;
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

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Swapchain created");
}

void E_Vk_MakeSync()
{
    VkResult result;

    VkFenceCreateInfo fence_info = { 0 };
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    result = vkCreateFence(rhi.device.handle, &fence_info, NULL, &rhi.command.upload_fence);
    assert(result == VK_SUCCESS);

    result = vkCreateFence(rhi.device.handle, &fence_info, NULL, &rhi.command.compute_fence);
    assert(result == VK_SUCCESS);

    fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++) {
        result = vkCreateFence(rhi.device.handle, &fence_info, NULL, &rhi.sync.fences[i]);
        assert(result == VK_SUCCESS);
    }

    VkSemaphoreCreateInfo semaphore_info = { 0 };
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    result = vkCreateSemaphore(rhi.device.handle, &semaphore_info, NULL, &rhi.sync.image_available_semaphore);
    assert(result == VK_SUCCESS);
    result = vkCreateSemaphore(rhi.device.handle, &semaphore_info, NULL, &rhi.sync.image_rendered_semaphore);
    assert(result == VK_SUCCESS);

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Sync Objects created");
}

void E_Vk_MakeCommand()
{
    VkCommandPoolCreateInfo pool_info = { 0 };
    pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    pool_info.queueFamilyIndex = rhi.physical_device.graphics_family;
    pool_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    VkResult result = vkCreateCommandPool(rhi.device.handle, &pool_info, NULL, &rhi.command.graphics_command_pool);
    assert(result == VK_SUCCESS);
    result = vkCreateCommandPool(rhi.device.handle, &pool_info, NULL, &rhi.command.upload_command_pool);
    assert(result == VK_SUCCESS);

    pool_info.queueFamilyIndex = rhi.physical_device.compute_family;

    result = vkCreateCommandPool(rhi.device.handle, &pool_info, NULL, &rhi.command.compute_command_pool);
    assert(result == VK_SUCCESS);

    rhi.command.swapchain_command_buffers = malloc(sizeof(VkCommandBuffer) * FRAMES_IN_FLIGHT);

    for (i32 i = 0; i < FRAMES_IN_FLIGHT; i++)
        rhi.command.swapchain_command_buffers[i] = E_CreateCommandBuffer(E_CommandBufferTypeGraphics);

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Command Objects created");
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

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Vulkan Allocator created");
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

void E_Vk_ImGuiCheckError(VkResult err)
{
    if (err == 0)
        return;
    E_LogError("VULKAN IMGUI ERROR: VkResult = %d", err);
    if (err < 0)
        abort();
}

void E_Vk_InitImGui()
{
    // Descriptor Pool

    VkDescriptorPoolSize pool_sizes[11] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 10000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 10000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 10000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 10000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 10000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 10000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 10000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 10000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 10000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 10000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 10000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {0};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 10000;
    pool_info.poolSizeCount = 11;
    pool_info.pPoolSizes = pool_sizes;

    VkResult result = vkCreateDescriptorPool(rhi.device.handle, &pool_info, NULL, &rhi.imgui.descriptor_pool);
    assert(result == VK_SUCCESS);
    result = vkCreateDescriptorPool(rhi.device.handle, &pool_info, NULL, &rhi.global_descriptor_pool);
    assert(result == VK_SUCCESS);

    // Render Pass

    VkAttachmentDescription color_attachment = {0};
    color_attachment.format = rhi.swapchain.image_format;
    color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
    color_attachment.loadOp = rhi_settings.gui_should_clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference color_attachment_ref = {0};
    color_attachment_ref.attachment = 0;
    color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {0};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &color_attachment_ref;

    VkSubpassDependency dependency = {0};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo render_pass_info = {0};
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    render_pass_info.attachmentCount = 1;
    render_pass_info.pAttachments = &color_attachment;
    render_pass_info.subpassCount = 1;
    render_pass_info.pSubpasses = &subpass;
    render_pass_info.dependencyCount = 1;
    render_pass_info.pDependencies = &dependency;

    result = vkCreateRenderPass(rhi.device.handle, &render_pass_info, NULL, &rhi.imgui.render_pass);
    assert(result == VK_SUCCESS);

    //

    ImGui_ImplVulkan_InitInfo init_info = { 0 };
    init_info.Instance = rhi.instance.handle;
    init_info.PhysicalDevice = rhi.physical_device.handle;
    init_info.Device = rhi.device.handle;
    init_info.QueueFamily = rhi.physical_device.graphics_family;
    init_info.Queue = rhi.device.graphics_queue;
    init_info.DescriptorPool = rhi.imgui.descriptor_pool;
    init_info.MinImageCount = 2;
    init_info.ImageCount = FRAMES_IN_FLIGHT;
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Subpass = 0;
    init_info.CheckVkResultFn = E_Vk_ImGuiCheckError;

    ImGui_ImplVulkan_Init(&init_info, rhi.imgui.render_pass);

    E_VulkanCommandBuffer* stc = E_Vk_CreateUploadCommandBuffer();
    ImGui_ImplVulkan_CreateFontsTexture(stc->handle);
    E_Vk_SubmitUploadCommandBuffer(stc);

    ImGui_ImplVulkan_DestroyFontUploadObjects();

    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        VkFramebufferCreateInfo fb_create_info = { 0 };
        fb_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_create_info.attachmentCount = 1;
        fb_create_info.pAttachments = &rhi.swapchain.image_views[i];
        fb_create_info.renderPass = rhi.imgui.render_pass;
        fb_create_info.width = rhi.swapchain.extent.width;
        fb_create_info.height = rhi.swapchain.extent.height;
        fb_create_info.layers = 1;

        result = vkCreateFramebuffer(rhi.device.handle, &fb_create_info, NULL, &rhi.imgui.swapchain_framebuffers[i]);
        assert(result == VK_SUCCESS);
    }
}

void E_Vk_RendererInit(E_Window* window, E_RendererInitSettings settings)
{
    rhi_settings = settings;

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
    E_Vk_InitImGui();

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Finished initialising the Vulkan renderer");
}

void E_Vk_RendererShutdown()
{
    ImGui_ImplVulkan_Shutdown();

    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        vkDestroyFramebuffer(rhi.device.handle, rhi.imgui.swapchain_framebuffers[i], NULL);
        E_FreeCommandBuffer(rhi.command.swapchain_command_buffers[i]);
    }
    free(rhi.command.swapchain_command_buffers);

    vkDestroyRenderPass(rhi.device.handle, rhi.imgui.render_pass, NULL);
    vkDestroyDescriptorPool(rhi.device.handle, rhi.imgui.descriptor_pool, NULL);
    vkDestroyDescriptorPool(rhi.device.handle, rhi.global_descriptor_pool, NULL);

    vmaDestroyAllocator(rhi.allocator);

    vkDestroySemaphore(rhi.device.handle, rhi.sync.image_available_semaphore, NULL);
    vkDestroySemaphore(rhi.device.handle, rhi.sync.image_rendered_semaphore, NULL);
    vkDestroyCommandPool(rhi.device.handle, rhi.command.upload_command_pool, NULL);
    vkDestroyCommandPool(rhi.device.handle, rhi.command.compute_command_pool, NULL);
    vkDestroyCommandPool(rhi.device.handle, rhi.command.graphics_command_pool, NULL);
    
    vkDestroyFence(rhi.device.handle, rhi.command.compute_fence, NULL);
    vkDestroyFence(rhi.device.handle, rhi.command.upload_fence, NULL);

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
    vkDestroyDebugUtilsMessengerEXT(rhi.instance.handle, rhi.instance.debug_messenger, NULL);
    vkDestroyInstance(rhi.instance.handle, NULL);

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Terminated the Vulkan Renderer");
}

void E_Vk_Begin()
{
    E_VulkanCommandBuffer* cmd_buf = CURRENT_CMD_BUF;

    vkAcquireNextImageKHR(rhi.device.handle, rhi.swapchain.handle, UINT32_MAX, rhi.sync.image_available_semaphore, VK_NULL_HANDLE, &rhi.sync.image_index);

    vkWaitForFences(rhi.device.handle, 1, &rhi.sync.fences[rhi.sync.image_index], VK_TRUE, UINT32_MAX);
    vkResetFences(rhi.device.handle, 1, &rhi.sync.fences[rhi.sync.image_index]);
    vkResetCommandBuffer(cmd_buf->handle, 0);

    E_BeginCommandBuffer(rhi.command.swapchain_command_buffers[rhi.sync.image_index]);
}

void E_Vk_End()
{
    E_VulkanCommandBuffer* cmd_buf = CURRENT_CMD_BUF;

    E_EndCommandBuffer(rhi.command.swapchain_command_buffers[rhi.sync.image_index]);

    // Submit

    VkSubmitInfo submit_info = { 0 };
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore wait_semaphores[] = { rhi.sync.image_available_semaphore };
    VkPipelineStageFlags wait_stages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submit_info.waitSemaphoreCount = 1;
    submit_info.pWaitSemaphores = wait_semaphores;
    submit_info.pWaitDstStageMask = wait_stages;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buf->handle;

    VkSemaphore signal_semaphores[] = { rhi.sync.image_rendered_semaphore };
    submit_info.signalSemaphoreCount = 1;
    submit_info.pSignalSemaphores = signal_semaphores;

    vkResetFences(rhi.device.handle, 1, &rhi.sync.fences[rhi.sync.image_index]);

    VkResult result = vkQueueSubmit(rhi.device.graphics_queue, 1, &submit_info, rhi.sync.fences[rhi.sync.image_index]);
    assert(result == VK_SUCCESS);
}

void E_Vk_Present()
{
    VkSemaphore signal_semaphores[] = { rhi.sync.image_rendered_semaphore };
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

void E_Vk_DrawMemoryUsageGUI()
{
    VmaStats stats;
    vmaCalculateStats(rhi.allocator, &stats);

    igText("Used: %d mb", stats.total.usedBytes / 1024 / 1024);
    igText("Unused: %d mb", stats.total.unusedBytes / 1024 / 1024);
    igText("Allocation Count: %d", stats.total.allocationCount);
    igText("Memory block count: %d", stats.total.blockCount);
}

void E_Vk_DrawGraphicsCardInfo()
{
    igText("GPU Name: %s", rhi.physical_device.handle_props.properties.deviceName);

    b32 instance_list = igTreeNodeEx_Str("Enabled Instance Extensions", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
    if (instance_list)
    {
        for (i32 i = 0; i < rhi.instance.extension_count; i++)
            igText("- %s", rhi.instance.extensions[i]);

        igTreePop();
    }

    b32 extension_list = igTreeNodeEx_Str("Enabled Device Extensions", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
    if (extension_list)
    {
        for (i32 i = 0; i < rhi.device.extension_count; i++)
            igText("- %s", rhi.device.extensions[i]);

        igTreePop();
    }

    b32 device_limits = igTreeNodeEx_Str("Device Limits", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
    if (device_limits)
    {
        igText("maxSamplerAnisotropy: %f", rhi.physical_device.handle_props.properties.limits.maxSamplerAnisotropy);
        igText("maxBoundDescriptorSets: %u", rhi.physical_device.handle_props.properties.limits.maxBoundDescriptorSets);
        igText("maxComputeWorkGroupCount: [%u ; %u ; %u]", rhi.physical_device.handle_props.properties.limits.maxComputeWorkGroupCount[0], rhi.physical_device.handle_props.properties.limits.maxComputeWorkGroupCount[1], rhi.physical_device.handle_props.properties.limits.maxComputeWorkGroupCount[2]);
        igText("maxComputeWorkGroupInvocations: %u", rhi.physical_device.handle_props.properties.limits.maxComputeWorkGroupInvocations);
        igText("maxComputeWorkGroupSize: [%u ; %u ; %u]", rhi.physical_device.handle_props.properties.limits.maxComputeWorkGroupSize[0], rhi.physical_device.handle_props.properties.limits.maxComputeWorkGroupSize[1], rhi.physical_device.handle_props.properties.limits.maxComputeWorkGroupSize[2]);
        igText("maxDrawIndirectCount: %u", rhi.physical_device.handle_props.properties.limits.maxDrawIndirectCount);
        igText("timestampPeriod: %f", rhi.physical_device.handle_props.properties.limits.timestampPeriod);

        igTreePop();
    }

    b32 mesh_shader_limits = igTreeNodeEx_Str("Mesh Shader Limits", ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_AllowItemOverlap | ImGuiTreeNodeFlags_FramePadding);
    if (mesh_shader_limits)
    {
        igText("maxDrawMeshTasksCount: %u", rhi.physical_device.mesh_shader_props.maxDrawMeshTasksCount);
        igText("maxMeshOutputPrimitives: %u", rhi.physical_device.mesh_shader_props.maxMeshOutputPrimitives);
        igText("maxMeshOutputVertices: %u", rhi.physical_device.mesh_shader_props.maxMeshOutputVertices);
        igText("maxMeshTotalMemorySize: %u", rhi.physical_device.mesh_shader_props.maxMeshTotalMemorySize);
        igText("maxMeshWorkGroupInvocations: %u", rhi.physical_device.mesh_shader_props.maxMeshWorkGroupInvocations);
        igText("maxMeshWorkGroupSize: [%u ; %u ; %u]", rhi.physical_device.mesh_shader_props.maxMeshWorkGroupSize[0], rhi.physical_device.mesh_shader_props.maxMeshWorkGroupSize[1], rhi.physical_device.mesh_shader_props.maxMeshWorkGroupSize[2]);
        igText("maxTaskOutputCount: %u", rhi.physical_device.mesh_shader_props.maxTaskOutputCount);
        igText("maxTaskTotalMemorySize: %u", rhi.physical_device.mesh_shader_props.maxTaskTotalMemorySize);
        igText("maxTaskWorkGroupInvocations: %u", rhi.physical_device.mesh_shader_props.maxTaskWorkGroupInvocations);
        igText("maxTaskWorkGroupSize: [%u ; %u ; %u]", rhi.physical_device.mesh_shader_props.maxTaskWorkGroupSize[0], rhi.physical_device.mesh_shader_props.maxTaskWorkGroupSize[1], rhi.physical_device.mesh_shader_props.maxTaskWorkGroupSize[2]);
        igText("meshOutputPerPrimitiveGranularity: %u", rhi.physical_device.mesh_shader_props.meshOutputPerPrimitiveGranularity);
        igText("meshOutputPerVertexGranularity: %u", rhi.physical_device.mesh_shader_props.meshOutputPerVertexGranularity);

        igTreePop();
    }
}

void E_Vk_BeginGUI()
{
    E_VulkanCommandBuffer* cmd_buf = CURRENT_CMD_BUF;

    VkClearValue clear_value = { 0 };

    VkRenderPassBeginInfo render_pass_info = { 0 };
    render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_info.renderPass = rhi.imgui.render_pass;
    render_pass_info.framebuffer = rhi.imgui.swapchain_framebuffers[rhi.sync.image_index];
    render_pass_info.renderArea.offset.x = 0;
    render_pass_info.renderArea.offset.y = 0;
    render_pass_info.renderArea.extent = rhi.swapchain.extent;
    render_pass_info.clearValueCount = 1;
    render_pass_info.pClearValues = &clear_value;

    vkCmdBeginRenderPass(cmd_buf->handle, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplWin32_NewFrame();
    igNewFrame();
}

void E_Vk_EndGUI()
{
    E_VulkanCommandBuffer* cmd_buf = CURRENT_CMD_BUF;

    igRender();
    ImGui_ImplVulkan_RenderDrawData(igGetDrawData(), cmd_buf->handle, VK_NULL_HANDLE);

    if (igGetIO()->ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        igUpdatePlatformWindows();
        igRenderPlatformWindowsDefault(NULL, NULL);
    }

    vkCmdEndRenderPass(cmd_buf->handle);
}

E_Image* E_Vk_GetSwapchainImage()
{
    return rhi.swapchain.euphorbe_images[rhi.sync.image_index];
}

u32 E_Vk_GetSwapchainImageIndex()
{
    return rhi.sync.image_index;
}

E_CommandBuffer* E_Vk_GetSwapchainCommandBuffer()
{
    return rhi.command.swapchain_command_buffers[rhi.sync.image_index];
}

void E_Vk_Resize(i32 width, i32 height)
{
    E_Vk_DeviceWait();

    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        free(rhi.swapchain.euphorbe_images[i]->rhi_handle);
        free(rhi.swapchain.euphorbe_images[i]);
        vkDestroyImageView(rhi.device.handle, rhi.swapchain.image_views[i], NULL);
        vkDestroyFramebuffer(rhi.device.handle, rhi.imgui.swapchain_framebuffers[i], NULL);
    }

    vkDestroySwapchainKHR(rhi.device.handle, rhi.swapchain.handle, NULL);
    free(rhi.swapchain.images);

    E_Vk_MakeSwapchain();

    for (u32 i = 0; i < FRAMES_IN_FLIGHT; i++)
    {
        VkFramebufferCreateInfo fb_create_info = { 0 };
        fb_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fb_create_info.attachmentCount = 1;
        fb_create_info.pAttachments = &rhi.swapchain.image_views[i];
        fb_create_info.renderPass = rhi.imgui.render_pass;
        fb_create_info.width = rhi.swapchain.extent.width;
        fb_create_info.height = rhi.swapchain.extent.height;
        fb_create_info.layers = 1;

        VkResult result = vkCreateFramebuffer(rhi.device.handle, &fb_create_info, NULL, &rhi.imgui.swapchain_framebuffers[i]);
        assert(result == VK_SUCCESS);
    }

    if (rhi_settings.log_renderer_events)
        E_LogInfo("RENDERER EVENT: Recreated swapchain with new dimensions: {%d, %d}", width, height);
}