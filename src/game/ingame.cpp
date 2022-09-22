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

ark::entity TestGround;
ark::entity TestGround2;
ark::entity TestObject;
ark::entity TestObject2;

std::vector<ark::entity> cricles;

void ingame::init()
{
	TestGround = ark::entities::create_phys_ground_entity(true, { 10, 0 }, { 1000, 10 });
	TestObject = ark::entities::create_phys_body_entity(true, { 250, 50 }, { 20, 10 });
	TestObject2 = ark::entities::create_phys_body_entity(true, { 350, 100 }, { 100, 10 });

	std::random_device r_device;
	std::mt19937 gen(r_device());
	for (size_t i = 0; i < 500; i++) {
		std::uniform_real_distribution width_dist(20., 1000.);
		std::uniform_real_distribution height_dist(20., 1000.);
		cricles.push_back(ark::entities::create_phys_body_entity_circle(true, { float(width_dist(gen)), float(height_dist(gen)) }, { 25, 25 }, ark::physics::material::material_type::rubber));
	}
	
	TestGround2 = ark::entities::create_phys_ground_entity(true, { 1000, 0 }, { 10, 500 });
	TestGround2 = ark::entities::create_phys_ground_entity(true, { 0, 0 }, { 10, 500 });
}