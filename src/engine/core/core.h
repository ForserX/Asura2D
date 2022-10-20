#pragma once

namespace ark::core
{
	void init();
	void destroy();

	int64_t get_cmd_int(stl::string_view key);
	stl::string_view get_cmd_string(stl::string_view key);
}

extern std::atomic_bool is_phys_ticking;
extern std::atomic_bool is_game_ticking;

extern std::atomic_uint8_t serialization_ref_counter;
extern std::atomic_uint8_t serialization_state;

enum entities_state : uint8_t
{
	idle,				// entity system doesn't do any work right now
	reading,			// entity system reading entities state
	writing,			// entity system writing entities state
	cleaning_up,		// entity system cleaning up all this shit
	viewing				// entity system waiting until other thread is doing his job
};