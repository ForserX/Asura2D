#pragma once
#include "asura_config.h"

#include <cstdint>
#include <set>
#include <map>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <fstream>
#include <random>
#include <variant>
#include <type_traits>
#include <concepts>
#include <algorithm>
#include <thread>
#include <execution>
#include <filesystem>
#include <mutex>
#include <sstream>

#if 0
// C++23 trash
#include <stacktrace>
#include <expected>
#else
#include <exception>
#endif

#ifdef OS_WINDOWS
#include <format>
#else
#include <fmt/format.h>
#endif

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <Intrin.h>
#include <timeapi.h>

// AVX/SSE trash
#include <xmmintrin.h>

#else
#if defined(OS_LINUX) || defined(OS_BSD)
#	include <sys/ptrace.h>
#endif
#ifdef OS_SOLARIS
#	include <alloca.h>
#	include <sys/unistd.h>
#endif
#ifdef OS_APPLE_SERIES
#	include <assert.h>
#	include <stdbool.h>
#	include <sys/types.h>
#	include <sys/sysctl.h>
#	include <mach/thread_policy.h>
#endif
#ifdef OS_UNIX
#	include <iostream>
#	include <cxxabi.h>
#	include <unistd.h>
#	include <sys/wait.h>
#endif
#include <pthread.h>
#endif

#include <visit_struct/visit_struct.hpp>

#include <entt/entt.hpp>
#include <fu2/function2.hpp>
#include "engine/render/opengl/glad.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>
#include <SOIL2.h>

#ifdef ARK_VULKAN
#include <SDL_vulkan.h>
#endif

#ifdef MIMALLOC_ENABLED
#include <mimalloc.h>
#endif

#include <box2d/box2d.h>
#include <box2d/b2_math.h>

#ifdef OPTICK_ENABLED
#include <optick.h>
#else
#define OPTICK_EVENT(a)
#define OPTICK_THREAD(a)
#define OPTICK_FRAME(a)
#endif

#ifdef ASURA_VULKAN
#include <vulkan.hpp>
#endif

// Audio trash
#ifdef OS_WINDOWS
#include <xaudio2.h>
#endif

#ifdef ASURA_USE_VORBIS
#	include <vorbis/include/vorbis/vorbisfile.h>
#else
#include <opusfile.h>
#endif

#include <opus.h>
#include <ogg/include/ogg/os_types.h>
#include <ogg/include/ogg/ogg.h>

#include <openal-soft/include/AL/al.h>
#include <openal-soft/include/AL/alc.h>

// Engine Base
#include "asura_engine.h"
#include "asura_global_vars.h"
