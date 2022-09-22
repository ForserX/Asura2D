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
ark::entity TestGround3;
ark::entity TestObject;
ark::entity TestObject2;

std::vector<ark::entity> cricles;

void ingame::init()
{

	TestGround = ark::entities::create_phys_ground_entity(true, { 10, 0 }, { 1500, 10 });
	TestObject = ark::entities::create_phys_body_entity(true, { 50, 50 }, { 20, 10 });
	TestObject2 = ark::entities::create_phys_body_entity(true, { 50, 100 }, { 200, 10 });

	for (size_t Iter = 0; Iter < 40; Iter++) {
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2 ) * 30, 450 }, { 15, 15 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2 ) * 20, 550 }, { 25, 25 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2 ) * 30, 650 }, { 35, 35 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2 ) * 20, 750 }, { 25, 25 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2 ) * 30, 850 }, { 25, 25 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2 ) * 20, 950 }, { 25, 25 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2) * 20, 1050 }, { 55, 55 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2) * 30, 1150 }, { 25, 25 }, ark::physics::material::material_type::rubber));
		cricles.push_back(ark::entities::create_phys_body_entity_cricle(true, { (float(Iter) / 2) * 20, 1250 }, { 25, 25 }, ark::physics::material::material_type::rubber));
	}

	TestGround2 = ark::entities::create_phys_ground_entity(true, { 1500, 0 }, { 10, 100 });
	TestGround3 = ark::entities::create_phys_ground_entity(true, { 0, 0 }, { 10, 100 });
}