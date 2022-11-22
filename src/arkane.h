#pragma once
#include "ark_config.h"

#include <cstdint>
#include <set>
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

#ifdef __linux__
#include <fmt/format.h>
#else
#include <format>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <timeapi.h>
#else
#ifdef __GNUC__
#include <cxxabi.h>
#endif
#include <pthread.h>
#endif

#include <boost/pfr/pfr.hpp>
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
