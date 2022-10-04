#pragma once
#include "ark_config.h"

#include <cstdint>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <format>
#include <fstream>
#include <random>
#include <variant>
#include <type_traits>
#include <concepts>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <timeapi.h>
#else
#include <pthread.h>
#endif

#include <entt/entt.hpp>
#include <fu2/function2.hpp>

#include <marl/defer.h>
#include <marl/event.h>
#include <marl/scheduler.h>
#include <marl/waitgroup.h>

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

#include "engine/types.h"
#include "engine/debug_core.h"
#include "engine/core.h"
#include "engine/logic_parser.h"

#include "engine/threads.h"
#include "engine/filesystem.h"
#include "engine/console.h"

#include "engine/physics_debug_draw.h"
#include "engine/physics_material.h"
#include "engine/physics_world.h"
#include "engine/physics.h"

#include "engine/components.h"
#include "engine/entities.h"

#include "engine/systems.h"
#include "engine/physics_system.h"
#include "engine/physics_mouse_joint_system.h"
#include "engine/draw_system.h"

#include "engine/camera.h"
#include "engine/level.h"

#include "engine/event.h"
#include "engine/input.h"
#include "engine/window.h"
#include "engine/ui.h"
#include "engine/graphics.h"
#include "engine/render.h"
#include "engine/application.h"

#include "engine/engine.h"
#include "engine/game.h"

extern bool is_phys_ticking;
extern bool is_game_ticking;
extern bool is_serializer_ticking;