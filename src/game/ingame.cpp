#include "ingame.h"

using namespace ark::systems;

std::vector<std::unique_ptr<ark::system>> pre_game_update_systems;
std::vector<std::unique_ptr<ark::system>> game_update_systems;
std::vector<std::unique_ptr<ark::system>> post_game_update_systems;
std::vector<std::unique_ptr<ark::system>> game_physics_systems;
std::vector<std::unique_ptr<ark::system>> game_draw_systems;

void init_systems()
{
	game_physics_systems.push_back(std::make_unique<ingame::movement_system>());
}

void ingame::pre_init()
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

	for (auto& system : game_physics_systems) {
		add_system(system.get(), update_type::physics_schedule);
	}
	
	for (auto& system : game_draw_systems) {
		add_system(system.get(), update_type::draw_schedule);
	}
}

ark::entity_view TestObject;
ark::entity_view TestObject2;

std::vector<ark::entity_view> circles;

void ingame::init()
{
	ark::level::init();
	ark::event::init();
	
	TestObject = ark::entities::create_phys_body(true, { 50, 50 }, { 20, 10 });
	TestObject2 = ark::entities::create_phys_body(true, { 50, 100 }, { 200, 10 });

	std::random_device r_device;
	std::mt19937 gen(r_device());
	for (size_t i = 0; i < 2000; i++) {
		std::uniform_real_distribution width_dist(20., 1000.);
		std::uniform_real_distribution height_dist(20., 1000.);
		circles.push_back(
			ark::entities::create_phys_body(
				true,
				{ static_cast<float>(width_dist(gen)), static_cast<float>(height_dist(gen)) },
				{ 25, 25 },
				ark::physics::body_type::around_body,
				ark::material::type::rubber
			)
		);
	}
}