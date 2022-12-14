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

#if defined(__FREEBSD__)
#define OS_BSD
#endif

#if !defined(OS_LINUX) || !defined(OS_BSD)
    #define ASURA_ALLOCATOR_USE
#endif

//#define ASURA_VULKAN
//#define ASURA_DX12
//#define ASURA_USE_STD_CONTAINERS