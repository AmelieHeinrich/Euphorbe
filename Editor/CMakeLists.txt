# CMake Header
cmake_minimum_required(VERSION 3.8)

# Source files
file(GLOB_RECURSE EDITOR_SOURCE Source/*.c Source/*.h)
add_executable(Editor ${EDITOR_SOURCE})

# Project settings
add_definitions(-DCIMGUI_DEFINE_ENUMS_AND_STRUCTS)
target_include_directories(Editor PUBLIC 
                           "Euphorbe"
                           ${THIRD_PARTY}/cimgui 
                           ${THIRD_PARTY}/cimgui/imgui)
target_link_libraries(Editor PUBLIC Euphorbe cimgui)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set_target_properties(Editor PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})