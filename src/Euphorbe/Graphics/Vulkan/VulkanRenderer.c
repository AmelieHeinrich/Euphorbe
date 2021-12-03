#include "VulkanRenderer.h"

#ifdef EUPHORBE_WINDOWS
    #include <vulkan/vulkan_win32.h>
    #include <Euphorbe/Platform/Windows/WindowsWindow.h>
#endif

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
            rhi.instance.layer_count = ARRAY_SIZE(instance_validation_layers);
            rhi.instance.layers[0] = "VK_LAYER_KHRONOS_validation";
            validation_layer_count = 1;
        }

        free(instance_layers);
    }

     VkBool32 surfaceExtFound = 0;
     VkBool32 platformSurfaceExtFound = 0;
     memset(rhi.instance.extensions, 0, sizeof(rhi.instance.extensions));
     result = vkEnumerateInstanceExtensionProperties(NULL, &instance_extension_count, NULL);
     assert(result == VK_SUCCESS);

    if (instance_extension_count > 0) {
        VkExtensionProperties *instance_extensions = malloc(sizeof(VkExtensionProperties) * instance_extension_count);
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

    VkApplicationInfo appInfo = {0};
    appInfo.pNext = NULL;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = rhi.window->title;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Euphorbe";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {0};
    createInfo.pNext = NULL;
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifdef _DEBUG
    createInfo.enabledLayerCount = rhi.instance.layer_count;
    createInfo.ppEnabledLayerNames = (const char *const *)rhi.instance.layers;
#else
    createInfo.enabledLayerCount = 0; 
    createInfo.ppEnabledLayerNames = NULL; 
#endif

    createInfo.enabledExtensionCount = rhi.instance.extension_count;
    createInfo.ppEnabledExtensionNames = (const char *const *)rhi.instance.extensions;

    result = vkCreateInstance(&createInfo, NULL, &rhi.instance.handle);
    assert(result == VK_SUCCESS);

    volkLoadInstance(rhi.instance.handle);
}

void E_Vk_MakeSurface()
{
#ifdef EUPHORBE_WINDOWS
    E_WindowsWindow* window = (E_WindowsWindow*)rhi.window->platform_data;
    HWND hwnd = window->hwnd;

    VkWin32SurfaceCreateInfoKHR surface_create_info = {0};
    surface_create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    surface_create_info.hinstance = GetModuleHandle(NULL);
    surface_create_info.hwnd = hwnd;
    surface_create_info.flags = 0;
    surface_create_info.pNext = NULL;

    // Load it myself because Volk is annoying
    PFN_vkCreateWin32SurfaceKHR function_pointer = vkGetInstanceProcAddr(rhi.instance.handle, "vkCreateWin32SurfaceKHR");

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
    vkEnumeratePhysicalDevices(rhi.instance.handle, &device_count, devices);
    rhi.physical_device.handle = devices[0];
    free(devices);

    vkGetPhysicalDeviceProperties(rhi.physical_device.handle, &rhi.physical_device.handle_props);

    // Find queue families
    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(rhi.physical_device.handle, &queue_family_count, NULL);

    VkQueueFamilyProperties* queue_families = malloc(sizeof(VkQueueFamilyProperties) * queue_family_count);
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

void E_Vk_MakeDevice()
{
    rhi.device.extension_count = 0;

    f32 queuePriority = 1.0f;

    VkDeviceQueueCreateInfo graphics_queue_create_info;
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
    vkEnumerateDeviceExtensionProperties(rhi.physical_device.handle, NULL, &extension_count, properties);
    
    for (int i = 0; i < extension_count; i++)
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

void E_Vk_RendererInit(E_Window* window)
{
    rhi.window = window;
    
    VkResult result = volkInitialize();
    assert(result == VK_SUCCESS);

    E_Vk_MakeInstance();
    E_Vk_MakeSurface();
    E_Vk_MakePhysicalDevice();
    E_Vk_MakeDevice();
}

void E_Vk_RendererShutdown()
{
    vkDestroyDevice(rhi.device.handle, NULL);
    vkDestroySurfaceKHR(rhi.instance.handle, rhi.surface, NULL);
    vkDestroyInstance(rhi.instance.handle, NULL);
}