#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>
#include <SDL_vulkan.h>

namespace ark::graphics {

void init();
void init_vulkan();

void destroy();
void tick(float dt);
	
}
