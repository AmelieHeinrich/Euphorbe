# CMake Header
cmake_minimum_required(VERSION 3.8)

# Get source files
file(GLOB_RECURSE EUPHORBE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.h)
file(GLOB_RECURSE EUPHORBE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.c ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.cpp)

# Project settings
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DVK_NO_PROTOTYPES)
add_library(Euphorbe ${EUPHORBE_HEADER} ${EUPHORBE_SOURCE})
target_include_directories(Euphorbe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../ThirdParty/Vulkan-Headers/include ../ThirdParty/volk ../ThirdParty/vma)
target_link_libraries(Euphorbe PUBLIC volk)

# Platform dependent settings
if (WIN32)
    set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
    add_definitions(-DVK_USE_PLATFORM_WIN32_KHR)
endif()