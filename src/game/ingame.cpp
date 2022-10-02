#include "ingame.h"

using namespace ark::systems;

ark::stl::vector<std::unique_ptr<ark::system>> pre_game_update_systems;
ark::stl::vector<std::unique_ptr<ark::system>> game_update_systems;
ark::stl::vector<std::unique_ptr<ark::system>> post_game_update_systems;
ark::stl::vector<std::unique_ptr<ark::system>> game_physics_systems;
ark::stl::vector<std::unique_ptr<ark::system>> game_draw_systems;

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

ark::stl::vector<ark::entity_view> circles;

void ingame::init()
{
	using namespace ark;
	using namespace entities;
	
	TestObject = add_phys_body(create(), {}, { 50, 50 }, { 20, 10 });
	TestObject2 = add_phys_body(create(), {}, { 350, 100 }, { 200, 10 });

	add_field<drawable_flag>(TestObject);
	add_field<drawable_flag>(TestObject2);
	
	std::random_device r_device;
	std::mt19937 gen(r_device());
	for (size_t i = 0; i < 2000; i++) {
		std::uniform_real_distribution width_dist(260., 800.);
		std::uniform_real_distribution height_dist(260., 800.);
		auto& ent = circles.emplace_back(
			add_phys_body(
				create(),
				{},
				{ static_cast<float>(width_dist(gen)), static_cast<float>(height_dist(gen)) },
				{ 20, 20 },
				physics::body_type::dynamic_body,
				physics::body_shape::circle_shape,
				material::type::rubber
			)
		);

		add_field<drawable_flag>(ent);
	}
}