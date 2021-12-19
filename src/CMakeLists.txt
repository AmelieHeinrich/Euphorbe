# CMake Header
cmake_minimum_required(VERSION 3.8)

# Get source files
file(GLOB_RECURSE EUPHORBE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.h)
file(GLOB_RECURSE EUPHORBE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.c ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.cpp)

find_package(Vulkan)
set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty)

# Project settings
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DVK_NO_PROTOTYPES)
add_library(Euphorbe ${EUPHORBE_HEADER} ${EUPHORBE_SOURCE})

target_include_directories(Euphorbe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} 
                                           ${THIRD_PARTY}/Vulkan-Headers/include 
                                           ${THIRD_PARTY}/volk 
                                           ${THIRD_PARTY}/vma 
                                           ${THIRD_PARTY}/spirv_reflect 
                                           ${THIRD_PARTY}/cimgui 
                                           ${THIRD_PARTY}/cimgui/imgui
                                           ${THIRD_PARTY}/cimgui/imgui/backends
                                           $ENV{VULKAN_SDK}/Include)

target_link_libraries(Euphorbe PUBLIC volk cimgui imgui_impl)
target_link_libraries(Euphorbe PUBLIC $ENV{VULKAN_SDK}/Lib/shaderc_combined.lib)
target_compile_definitions(Euphorbe PUBLIC IMGUI_IMPL_API=)

# Platform dependent settings
if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
endif()