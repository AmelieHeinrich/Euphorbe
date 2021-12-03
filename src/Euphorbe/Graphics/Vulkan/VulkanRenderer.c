#include "VulkanRenderer.h"

#ifdef EUPHORBE_WINDOWS
    #include <vulkan/vulkan_win32.h>
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

            if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instance_extensions[i].extensionName)) {
                rhi.instance.extensions[rhi.instance.extension_count++] = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
            }

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

void E_Vk_RendererInit(E_Window* window)
{
    rhi.window = window;
    
    VkResult result = volkInitialize();
    assert(result == VK_SUCCESS);

    E_Vk_MakeInstance();
}

void E_Vk_RendererShutdown()
{
    vkDestroyInstance(rhi.instance.handle, NULL);
}