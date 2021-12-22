# CMake Header
cmake_minimum_required(VERSION 3.8)

# Get source files
file(GLOB_RECURSE EUPHORBE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.h ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.c ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.cpp)

find_package(Vulkan)
set(THIRD_PARTY ${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParty)

# Project settings
add_definitions(-DCIMGUI_DEFINE_ENUMS_AND_STRUCTS)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DVK_NO_PROTOTYPES)
add_library(Euphorbe ${EUPHORBE_SOURCE})

target_include_directories(Euphorbe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} 
                                           ${THIRD_PARTY}/Vulkan-Headers/include 
                                           ${THIRD_PARTY}/volk 
                                           ${THIRD_PARTY}/vma 
                                           ${THIRD_PARTY}/spirv_reflect 
                                           ${THIRD_PARTY}/cimgui 
                                           ${THIRD_PARTY}/cimgui/imgui
                                           ${THIRD_PARTY}/imgui_backends
                                           ${THIRD_PARTY}/tomlc99
                                           ${THIRD_PARTY}/stb
					   ${THIRD_PARTY}/cglm/include
                                           $ENV{VULKAN_SDK}/Include)

target_link_libraries(Euphorbe PUBLIC volk cimgui imgui_impl TOML cglm)
target_link_libraries(Euphorbe PUBLIC $ENV{VULKAN_SDK}/Lib/shaderc_combined.lib)
target_compile_definitions(Euphorbe PUBLIC IMGUI_IMPL_API=)

# Platform dependent settings
if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${EUPHORBE_SOURCE})