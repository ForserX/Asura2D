#include "pch.h"

#ifdef _WIN32
#include <Windows.h>
#else
#define DebugBreak
#endif

static std::ofstream log_file;

using namespace ark;

void
debug::init()
{
	std::filesystem::path log_path = filesystem::get_userdata_dir();
	log_path.append("user.log");

	filesystem::create_file(log_path);

	log_file.open(log_path);
}

void
debug::destroy()
{
	log_file.close();
}

void
debug::show_error(std::string_view message)
{
	print_message(message);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);

#ifdef _DEBUG
	DebugBreak();
#endif
}

void
debug::print_message(std::string_view message)
{
	log_file << message << std::endl;
	ui::push_console_string(message);

#if defined(_DEBUG) & defined(_WIN32)
	if (IsDebuggerPresent()) {
		OutputDebugString("Arkane2D: ");
		OutputDebugString(message.data());
		OutputDebugString("\r\n");
	}
#endif
}