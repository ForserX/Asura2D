#include "pch.h"

using namespace Asura;

bool is_started = false;

stl::hash_set<Asura::system*> pre_update_systems = {};
stl::hash_set<Asura::system*> update_systems = {};
stl::hash_set<Asura::system*> post_update_systems = {};

stl::hash_set<Asura::system*> draw_systems = {};
stl::hash_set<Asura::system*> physics_systems = {};

std::unique_ptr<Asura::system> engine_draw_system = {};
std::unique_ptr<Asura::system> engine_physics_system = {};
std::unique_ptr<Asura::system> engine_physics_scene_system = {};
std::unique_ptr<Asura::system> engine_physics_mouse_joint_system = {};

stl::hash_set<Asura::system*>* get_system_by_type(Systems::update_type type)
{
	switch (type)
	{
	case Systems::update_type::pre_update_schedule:
		return &pre_update_systems;
	case Systems::update_type::update_schedule:
		return &update_systems;
	case Systems::update_type::post_update_schedule:
		return &post_update_systems;
	case Systems::update_type::draw_schedule:
		return &draw_systems;
	case Systems::update_type::physics_schedule:
		return &physics_systems;
	}

	return nullptr;
}

void Systems::delete_system(system* system_to_delete, update_type type)
{
	stl::hash_set<system*>* systems_list = get_system_by_type(type);
	game_assert(systems_list->contains(system_to_delete), "Pointer not found", std::terminate());
	systems_list->erase(system_to_delete);
}

void Systems::add_system(system* system_to_add, update_type type)
{
	stl::hash_set<system*>* systems_list = get_system_by_type(type);
	game_assert(!systems_list->contains(system_to_add), "Pointer is alive! Duplicate!", std::terminate());
	systems_list->insert(system_to_add);

	if (is_started) {
		system_to_add->Init();
	}
}

void Systems::pre_init()
{
	engine_draw_system = std::make_unique<draw_system>();
	draw_systems.insert(engine_draw_system.get());

    engine_physics_scene_system = std::make_unique<physics_scene_system>();
    physics_systems.insert(engine_physics_scene_system.get());
    
	engine_physics_mouse_joint_system = std::make_unique<physics_mouse_joint_system>();
	physics_systems.insert(engine_physics_mouse_joint_system.get());
	
	engine_physics_system = std::make_unique<physics_system>();
	physics_systems.insert(engine_physics_system.get());
}

void Systems::Init()
{
	for	(const auto system : pre_update_systems) 
	{
		system->Init();
	}
	
	for	(const auto system : update_systems) 
	{
		system->Init();
	}

	for	(const auto system : post_update_systems) 
	{
		system->Init();
	}

	for	(const auto system : draw_systems) 
	{
		system->Init();
	}

	for	(const auto system : physics_systems) 
	{
		system->Init();
	}
	
	is_started = true;
}

void Systems::Destroy()
{
	is_started = false;
	
	for	(const auto system : pre_update_systems)
	{
		system->Reset();
	}
	
	for	(const auto system : update_systems) 
	{
		system->Reset();
	}

	for	(const auto system : post_update_systems) 
	{
		system->Reset();
	}

	for	(const auto system : draw_systems) 
	{
		system->Reset();
	}

	for	(const auto system : physics_systems) 
	{
		system->Reset();
	}
	
	pre_update_systems.clear();
	update_systems.clear();
	post_update_systems.clear();
	
	draw_systems.clear();
	physics_systems.clear();
}

void system_tick(float dt, const stl::hash_set<Asura::system*>& Systems)
{
	for	(const auto system : Systems)
	{
		system->Tick(dt);
	}
}

void Systems::pre_tick(float dt)
{
	system_tick(dt, pre_update_systems);
}

void Systems::Tick(float dt)
{
	system_tick(dt, update_systems);
}

void Systems::post_tick(float dt)
{
	system_tick(dt, post_update_systems);
}

void Systems::draw_tick(float dt)
{
	system_tick(dt, draw_systems);
}

void Systems::physics_tick(float dt)
{
	system_tick(dt, physics_systems);
}
