#pragma once

#include "engine/Core/tmagic.h"
#include "engine/Core/types.h"
#include "engine/Core/Math.h"
#include "engine/Core/treflect.h"

#include "engine/audio/audio.h"
#include "engine/Core/debug_core.h"
#include "engine/Core/Core.h"
#include "engine/Core/logic_parser.h"
#include "engine/Core/Threads.h"
#include "engine/Core/FileSystem.h"
#include "engine/Core/console.h"
#include "engine/Core/resources.h"
#include "engine/Core/scheduler.h"

#include "engine/physics/physics_debug_draw.h"
#include "engine/physics/physics_material.h"
#include "engine/physics/physics_body.h"
#include "engine/physics/physics_joint.h"
#include "engine/physics/physics_world.h"
#include "engine/physics/physics.h"

#include "engine/net/network.h"

#include "engine/eces/components.h"
#include "game_components.h"

#include "engine/eces/reflection.h"
#include "engine/eces/entities.h"
#include "engine/eces/entities_helpers.h"
#include "engine/eces/entities_serializer.h"

#include "engine/eces/event.h"
#include "engine/eces/level.h"
#include "engine/eces/scene.h"

#include "engine/systems/systems.h"
#include "engine/systems/network_system.h"
#include "engine/systems/physics_scene_system.h"
#include "engine/systems/physics_system.h"
#include "engine/systems/physics_mouse_joint_system.h"
#include "engine/systems/draw_system.h"

#include "engine/render/camera.h"
#include "engine/render/window.h"
#include "engine/render/ui.h"
#include "engine/render/graphics.h"
#include "engine/render/render.h"

#include "engine/engine/engine.h"
#include "engine/engine/input.h"
#include "engine/engine/game.h"

#include "engine/gameplay/gameplay_common.h"