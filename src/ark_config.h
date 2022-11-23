#pragma once

// OS Global Defines
#ifdef _WIN32
#define OS_WINDOWS
#endif

#ifdef __linux__
#define OS_LINUX
#define OS_UNIX
#endif

#ifdef __ANDROID__
#define OS_ANDROID
#define OS_UNIX
#endif

#ifdef __APPLE__
#define OS_APPLE_SERIES
#include <TargetConditionals.h>
#if TARGET_IPHONE_SIMULATOR | TARGET_OS_IPHONE
#   define OS_IOS
#elif TARGET_OS_MAC
#   define OS_MACOS
#endif

#define OS_UNIX
#endif

#ifndef OS_LINUX
    #define ARK_ALLOCATOR_USE
#endif

//#define ARK_VULKAN
//#define ARK_DX12