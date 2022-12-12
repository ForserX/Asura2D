#pragma once

namespace Asura::Core
{
	void Init();
	void Destroy();

	int64_t GetCmdInt(stl::string_view key);
	stl::string_view GetCmdStr(stl::string_view key);
}

extern std::atomic_bool is_phys_ticking;
extern std::atomic_bool is_game_ticking;

extern std::atomic_uint8_t serialization_ref_counter;
extern std::atomic_uint8_t serialization_state;

enum entities_state : uint8_t
{
	idle,				// entity system doesn't do any work right now
	reading,			// entity system reading Entities state
	writing,			// entity system writing Entities state
	cleaning_up,		// entity system cleaning up all this shit
	viewing				// entity system waiting until other thread is doing his job
};