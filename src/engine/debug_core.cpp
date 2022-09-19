#include "debug_core.h"

#include <SDL.h>
#include <format>

void ark::debug::show_error(std::string_view message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);
}

void ark::debug::print_message(std::string_view message)
{
	// maybe later
	//...
}