# CMake Header
project("Euphorbe")

cmake_minimum_required(VERSION 3.8)

# Settings
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Dependencies
add_subdirectory(ThirdParty/volk)

# Main Projects
add_subdirectory(src)
add_subdirectory(Sandbox)