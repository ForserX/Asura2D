#include "game.h"

using namespace ark::systems;

std::vector<std::unique_ptr<ark::system>> pre_game_update_systems;
std::vector<std::unique_ptr<ark::system>> game_update_systems;
std::vector<std::unique_ptr<ark::system>> post_game_update_systems;

void init_systems()
{
	game_update_systems.push_back(std::make_unique<game::movement_system>());
}

void game::pre_init()
{
	init_systems();

	for (auto& system : pre_game_update_systems) {
		add_system(system.get(), update_type::pre_update_schedule);
	}
	
	for (auto& system : game_update_systems) {
		add_system(system.get(), update_type::update_schedule);
	}

	for (auto& system : post_game_update_systems) {
		add_system(system.get(), update_type::post_update_schedule);
	}
}

void game::init()
{
	
}