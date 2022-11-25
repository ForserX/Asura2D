#pragma once
#include "ark_config.h"

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
#ifdef OS_UNIX
#include <iostream>
#include <cxxabi.h>
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

#ifdef ARK_VULKAN
#include <vulkan.hpp>
#endif

#include "arkane_engine.h"
