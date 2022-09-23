#include "pch.h"

using namespace ark;

void
application::init()
{
	constexpr Uint32 Flags = SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC | SDL_INIT_EVENTS;
	if (SDL_Init(Flags) != 0)
	{
		return;
	}

	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void
application::destroy()
{
	SDL_Quit();
}

void
application::tick(float dt)
{
}
