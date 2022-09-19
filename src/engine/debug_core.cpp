#include "arkane.h"
#include "debug_core.h"

#include <fstream>
#include <SDL.h>
#include <format>

static std::ofstream log_file;

void ark::debug::init()
{
	std::filesystem::path log_path = filesystem::get_userdata_dir();
	log_path.append("user.log");

	filesystem::create_file(log_path);

	log_file.open(log_path);
}

void ark::debug::destroy()
{
	log_file.close();
}

void ark::debug::show_error(std::string_view message)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);
}

void ark::debug::print_message(std::string_view message)
{
	log_file << message << std::endl;
}