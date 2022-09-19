#pragma once
#include <cstdint>
#include <set>
#include <vector>
#include <memory>
#include <string>
#include <string_view>
#include <format>
#include <fstream>

#include <entt/entity/registry.hpp>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>
#include <SDL.h>
#include <SDL_vulkan.h>
#include <box2d/box2d.h>
#include <box2d/b2_math.h>

#ifdef ARK_VULKAN
#include <vulkan.hpp>
#endif

#include "engine/types.h"
#include "engine/debug_core.h"
#include "engine/core.h"
#include "engine/logic_parser.h"

#include "engine/physical.h"
#include "engine/components.h"

#include "engine/filesystem.h"
#include "engine/systems.h"

#include "engine/window.h"
#include "engine/ui.h"
#include "engine/graphics.h"
#include "engine/application.h"
#include "engine/engine.h"