#pragma once
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"
#include <SDL.h>

namespace ark::graphics {

void init();
void destroy();
void tick(float dt);
	
}
