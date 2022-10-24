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

#include <format>

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

#include <boost/pfr.hpp>
#include <boost/hana.hpp>
#include <boost/hana/accessors.hpp>
#include <boost/hana/adapt_struct.hpp>
#include <boost/hana/assert.hpp>
#include <boost/hana/core/to.hpp>
#include <boost/hana/equal.hpp>
#include <boost/hana/find.hpp>
#include <boost/hana/first.hpp>
#include <boost/hana/map.hpp>
#include <boost/hana/not_equal.hpp>
#include <boost/hana/optional.hpp>
#include <boost/hana/pair.hpp>
#include <boost/hana/string.hpp>
#include <boost/hana/transform.hpp>
#include <boost/hana/tuple.hpp>

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
