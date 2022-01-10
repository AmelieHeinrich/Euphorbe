#pragma once

#include <stdint.h>
#include <memory.h>
#include <stdlib.h>
#include <assert.h>

// Typedefs
typedef char i8;
typedef unsigned char u8;
typedef short i16;
typedef unsigned short u16;
typedef int i32;
typedef unsigned int u32;
typedef long long i64;
typedef unsigned long long u64;
typedef u8 b8;
typedef u16 b16;
typedef u32 b32;
typedef float f32;
typedef double f64;

// Useful defines
#define ARRAY_SIZE(arr) sizeof(arr) / sizeof(arr[0])
#define KILOBYTES(bytes) bytes * 1024
#define MEGABYTES(bytes) KILOBYTES(bytes) * 1024
#define GIGABYTES(bytes) MEGABYTES(bytes) * 1024
#define OFFSET_PTR_BYTES(type, ptr, offset) ((type*)((u8*)ptr + (offset)))
#define FRAMES_IN_FLIGHT 3
#define PAD_SIZE_16(s) ( ((s) + 15) & ~15 )

// Platform detection
#ifdef _WIN32
    #define EUPHORBE_WINDOWS
#else
    #error "Platform not yet supported!"
#endif

// Platform includes

#ifdef EUPHORBE_WINDOWS
    #pragma warning(disable: 5105)
    #define WIN32_LEAN_AND_MEAN
    #include <Windows.h>
#endif