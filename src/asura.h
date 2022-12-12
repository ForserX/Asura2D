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

#ifdef OS_WINDOWS
#include <format>
#else
#include <fmt/format.h>
#endif

#ifdef OS_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <timeapi.h>
#else
#ifdef OS_LINUX
#	include <sys/ptrace.h>
#	include <sys/wait.h>
#endif
#ifdef OS_APPLE_SERIES
#	include <assert.h>
#	include <stdbool.h>
#	include <sys/types.h>
#	include <sys/sysctl.h>
#endif
#ifdef OS_UNIX
#	include <iostream>
#	include <cxxabi.h>
#	include <unistd.h>
#endif
#include <pthread.h>
#endif

#include <visit_struct/visit_struct.hpp>

#include <entt/entt.hpp>
#include <fu2/function2.hpp>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <imgui_internal.h>
#include <SDL.h>

#ifdef ARK_VULKAN
#include <SDL_vulkan.h>
#endif

#include <SDL_image.h>

#ifdef MIMALLOC_ENABLED
#include <mimalloc.h>
#endif

#include <box2d/box2d.h>
#include <box2d/b2_math.h>

#include <optick.h>

#ifdef ASURA_VULKAN
#include <vulkan.hpp>
#endif

#ifdef OS_WINDOWS
#include <xaudio2.h>
#endif

// Audio trash
#include <vorbis/include/vorbis/vorbisfile.h>
#include <ogg/include/ogg/os_types.h>
#include <ogg/include/ogg/ogg.h>

#include <openal-soft/include/AL/al.h>
#include <openal-soft/include/AL/alc.h>

#include "asura_engine.h"
