# Euphorbe

![Euphorbe](.github/logo.png)

Euphorbe is a fresh early-stage game engine written entirely in C.
It's purpose is mainly to try and build an efficient yet developped Vulkan renderer, and this with the usage of the **VK_KHR_dynamic_rendering** extension.
***

## Getting started

Note that only MSVC is supported to build Euphorbe!

**1. Download the repository**
```bat
git clone https://github.com/Sausty/Euphorbe
install.bat
```

**2. Generate the project**
```bat
cmake -G "Visual Studio 17 2022" -B build
```

## Dependencies

- [volk](https://github.com/zeux/volk)
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)
- [shaderc](https://github.com/google/shaderc)
- [VMA](https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator)
- [SPIRV-Reflect](https://github.com/KhronosGroup/SPIRV-Reflect)
- [cimgui](https://github.com/Sausty/cimgui)
- [tomlc99](https://github.com/cktan/tomlc99)
- [cglm](https://github.com/recp/cglm)
- [cgltf](https://github.com/jkuhlmann/cgltf)

## Screenshots

### Screenshot from Dec 29 2021
![Dec 29 2021](.github/29dec2021.PNG)

## Additional information

**Note that you need the Beta Vulkan drivers to run this program!**
