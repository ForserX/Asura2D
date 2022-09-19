#include "debug_core.h"

#include <SDL.h>
#include <format>

void ark::debug::show_error(std::string_view message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);
}

void ark::debug::logger::msg(std::string_view format, std::format_args args)
{
	std::string message = std::format(format, args);

	// maybe later
	//...
}
