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
	TestObject = ark::entities::add_phys_body(ark::entities::create_entity(), { 50, 50 }, { 20, 10 });
	TestObject2 = ark::entities::add_phys_body(ark::entities::create_entity(), { 350, 100 }, { 200, 10 });

	ark::entities::add_field<ark::entities::drawable_flag>(TestObject);
	ark::entities::add_field<ark::entities::drawable_flag>(TestObject2);
	
	std::random_device r_device;
	std::mt19937 gen(r_device());
	for (size_t i = 0; i < 4000; i++) {
		std::uniform_real_distribution width_dist(260., 800.);
		std::uniform_real_distribution height_dist(260., 800.);
		// add_field<drawable_flag>(ent);
		auto& ent = circles.emplace_back(
			ark::entities::add_phys_body(
				ark::entities::create_entity(),
				{ static_cast<float>(width_dist(gen)), static_cast<float>(height_dist(gen)) },
				{ 10, 10 },
				ark::physics::body_type::around_body,
				ark::material::type::rubber
			)
		);

		ark::entities::add_field<ark::entities::drawable_flag>(ent);
	}
}