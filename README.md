# Euphorbe

![Euphorbe](.github/logo.png)

Euphorbe is a fresh early-stage game engine written entirely in C.
It's purpose is mainly to try and build an efficient yet developped Vulkan renderer, and this with the usage of the **VK_KHR_dynamic_rendering** extension.
***

## Getting started

**1. Download the repository**
```bat
git clone --recursive https://github.com/Sausty/Euphorbe
```

**2. Generate the project**
```bat
cmake -G "(generator)" -B build
```

## Dependencies

- [volk](https://github.com/zeux/volk)
- [Vulkan-Headers](https://github.com/KhronosGroup/Vulkan-Headers)

## Additional information

**Note that you need the Beta Vulkan drivers to run this program!**