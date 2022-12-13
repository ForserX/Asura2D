#include "pch.h"

using namespace Asura;

std::atomic_bool is_phys_ticking = false;
std::atomic_bool is_game_ticking = false;

std::atomic_uint8_t serialization_ref_counter = 0;
std::atomic_uint8_t serialization_state = 0;

void Core::Init()
{
	FileSystem::Path log_path = FileSystem::UserdataDir();
	log_path.append("user.ini");

	FileSystem::CreateFile(log_path);
}

void Core::Destroy()
{
}

int64_t Core::GetCmdInt(stl::string_view key)
{
    return UI::GetCmdInt(key);
}
