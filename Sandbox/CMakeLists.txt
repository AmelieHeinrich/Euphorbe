# CMake Header
cmake_minimum_required(VERSION 3.8)

# Source files
add_executable(Sandbox Editor/main.c)

# Project settings
add_definitions(-DCIMGUI_DEFINE_ENUMS_AND_STRUCTS)
target_include_directories(Sandbox PUBLIC 
                           "Euphorbe"
                           ${THIRD_PARTY}/cimgui 
                           ${THIRD_PARTY}/cimgui/imgui)
target_link_libraries(Sandbox PUBLIC Euphorbe cimgui)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)