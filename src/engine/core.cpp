#include "pch.h"

using namespace ark;

std::atomic_bool is_phys_ticking = false;
std::atomic_bool is_game_ticking = false;

std::atomic_uint8_t serialization_ref_counter = 0;
std::atomic_uint8_t serialization_state = 0;

void
core::init()
{
	std::filesystem::path log_path = filesystem::get_userdata_dir();
	log_path.append("user.ini");

	filesystem::create_file(log_path);
}

void
core::destroy()
{
}

int64_t
core::get_cmd_int(stl::string_view key)
{
    return ui::get_cmd_int(key);
}
