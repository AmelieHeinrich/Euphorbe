# Euphorbe

![Euphorbe](.github/logo.png)

Euphorbe is a fresh early-stage game engine written entirely in C.
It's purpose is mainly to try and build an efficient yet developped Vulkan renderer, and this with the usage of the **VK_KHR_dynamic_rendering** extension.
***

## Getting started

Note that only MSVC is supported to build Euphorbe!

**1. Download the repository**
```bat
git clone --recursive https://github.com/Sausty/Euphorbe
```

**2. Generate the project**
```bat
cmake -G "Visual Studio 16 2019" -B build
```

Make sure to copy the Assets directory into build/Sandbox

## Dependencies

- [volk](https://github.com/zeux/volk)
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [shaderc](https://github.com/google/shaderc)
- [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [SPIRV-Reflect](https://github.com/KhronosGroup/SPIRV-Reflect)
- [cimgui](https://github.com/Sausty/cimgui)

## Additional information

**Note that you need the Beta Vulkan drivers to run this program!**