cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE EUPHORBE_HEADER ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.h)
file(GLOB_RECURSE EUPHORBE_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/Euphorbe/*.c)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

add_library(Euphorbe ${EUPHORBE_HEADER} ${EUPHORBE_SOURCE})
target_include_directories(Euphorbe PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})