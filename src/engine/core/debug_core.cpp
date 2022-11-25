#include "pch.h"

#ifdef OS_MACOS
#define DebugBreak __builtin_trap
#elif defined(OS_UNIX)
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
debug::show_error(stl::string_view message)
{
#ifdef _DEBUG
	DebugBreak();
#endif

	print_message(message);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error!", message.data(), nullptr);
}

void
debug::print_message(stl::string_view message)
{
	log_file << message << std::endl;
	ui::push_console_string(message);

#if defined(_DEBUG) & defined(OS_WINDOWS)
	if (IsDebuggerPresent()) {
		OutputDebugString("ark: ");
		OutputDebugString(message.data());
		OutputDebugString("\r\n");
	}
#endif
#if defined(OS_LINUX)
	std::cout << "ark: " << message.data() << std::endl;	
#endif

}