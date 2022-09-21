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

#include <entt/entt.hpp>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <imgui_internal.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <box2d/box2d.h>
#include <box2d/b2_math.h>

#ifdef WIN32
#include <windows.h>
#endif

#ifdef ARK_VULKAN
#include <vulkan.hpp>
#endif

#include "engine/types.h"
#include "engine/debug_core.h"
#include "engine/core.h"
#include "engine/logic_parser.h"

#include "engine/filesystem.h"
#include "engine/console.h"

#include "engine/components.h"
#include "engine/entities.h"

#include "engine/physical.h"
#include "engine/physics.h"

#include "engine/systems.h"
#include "engine/physics_system.h"
#include "engine/draw_system.h"

#include "engine/game.h"

#include "engine/window.h"
#include "engine/ui.h"
#include "engine/graphics.h"
#include "engine/render.h"
#include "engine/application.h"
#include "engine/engine.h"