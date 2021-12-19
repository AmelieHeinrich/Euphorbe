# CMake Header
project(Euphorbe LANGUAGES C CXX)

cmake_minimum_required(VERSION 3.8)

# Settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Dependency settings
set(IMGUI_STATIC "on")

# Dependencies
add_subdirectory(ThirdParty/volk)
add_subdirectory(ThirdParty/cimgui)

# ImGui impl
add_library(imgui_impl STATIC ThirdParty/cimgui/imgui/backends/imgui_impl_win32.cpp ThirdParty/cimgui/imgui/backends/imgui_impl_vulkan.cpp)
target_include_directories(imgui_impl PRIVATE ThirdParty/cimgui/imgui $ENV{VULKAN_SDK}/Include)
target_compile_definitions(imgui_impl PRIVATE IMGUI_IMPL_API=extern\ \"C\")
target_compile_definitions(imgui_impl PRIVATE VK_NO_PROTOTYPES)

# Main Projects
add_subdirectory(src)
add_subdirectory(Sandbox)