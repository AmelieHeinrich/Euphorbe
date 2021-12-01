# CMake Header
cmake_minimum_required(VERSION 3.8)

# Source files
add_executable(Sandbox Editor/main.c)

# Project settings
target_include_directories(Sandbox PUBLIC "Euphorbe")
target_link_libraries(Sandbox PUBLIC Euphorbe)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)