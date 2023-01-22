#include "pch.h"

using namespace Asura;

bool is_started = false;

stl::hash_set<Asura::ISystem*> pre_update_systems = {};
stl::hash_set<Asura::ISystem*> update_systems = {};
stl::hash_set<Asura::ISystem*> post_update_systems = {};

stl::hash_set<Asura::ISystem*> draw_systems = {};
stl::hash_set<Asura::ISystem*> physics_systems = {};

std::unique_ptr<Asura::ISystem> engine_draw_system = {};
std::unique_ptr<Asura::ISystem> engine_physics_system = {};
std::unique_ptr<Asura::ISystem> engine_physics_scene_system = {};

stl::hash_set<Asura::ISystem*>* get_system_by_type(Systems::UpdateType type)
{
	switch (type)
	{
	case Systems::UpdateType::befor:
		return &pre_update_systems;
	case Systems::UpdateType::update:
		return &update_systems;
	case Systems::UpdateType::after:
		return &post_update_systems;
	case Systems::UpdateType::render:
		return &draw_systems;
	case Systems::UpdateType::physics:
		return &physics_systems;
	}

	return nullptr;
}

void Systems::Unsubscribe(ISystem* System_to_delete, UpdateType type)
{
	stl::hash_set<ISystem*>* Systems_list = get_system_by_type(type);
	game_assert(Systems_list->contains(System_to_delete), "Pointer not found", std::terminate());
	Systems_list->erase(System_to_delete);
}

void Systems::Subscribe(ISystem* System_to_add, UpdateType type)
{
	stl::hash_set<ISystem*>* Systems_list = get_system_by_type(type);
	game_assert(!Systems_list->contains(System_to_add), "Pointer is alive! Duplicate!", std::terminate());
	Systems_list->insert(System_to_add);

	if (is_started) 
	{
		System_to_add->Init();
	}
}

void Systems::PreInit()
{
	engine_draw_system = std::make_unique<RenderSystem>();
	draw_systems.insert(engine_draw_system.get());

    engine_physics_scene_system = std::make_unique<SceneSystem>();
    physics_systems.insert(engine_physics_scene_system.get());
    
	engine_physics_system = std::make_unique<PhysicsSystem>();
	physics_systems.insert(engine_physics_system.get());
}

void Systems::Init()
{
	for	(const auto System : pre_update_systems) 
	{
		System->Init();
	}
	
	for	(const auto System : update_systems) 
	{
		System->Init();
	}

	for	(const auto System : post_update_systems) 
	{
		System->Init();
	}

	for	(const auto System : draw_systems) 
	{
		System->Init();
	}

	for	(const auto System : physics_systems) 
	{
		System->Init();
	}
	
	is_started = true;
}

void Systems::Destroy()
{
	is_started = false;
	
	for	(const auto System : pre_update_systems)
	{
		System->Reset();
	}
	
	for	(const auto System : update_systems) 
	{
		System->Reset();
	}

	for	(const auto System : post_update_systems) 
	{
		System->Reset();
	}

	for	(const auto System : draw_systems) 
	{
		System->Reset();
	}

	for	(const auto System : physics_systems) 
	{
		System->Reset();
	}
	
	pre_update_systems.clear();
	update_systems.clear();
	post_update_systems.clear();
	
	draw_systems.clear();
	physics_systems.clear();
}

inline void SystemTick(float dt, const stl::hash_set<Asura::ISystem*>& Systems)
{
	for	(const auto System : Systems)
	{
		System->Tick(dt);
	}
}

void Systems::BeforTick(float dt)
{
	SystemTick(dt, pre_update_systems);
}

void Systems::Tick(float dt)
{
	if (!is_editor_mode || EditorRealtimeMode)
	{
		SystemTick(dt, update_systems);
	}
}

void Systems::AfterTick(float dt)
{
	SystemTick(dt, post_update_systems);
}

void Systems::RenderTick(float dt)
{
	SystemTick(dt, draw_systems);
}

void Systems::PhysTick(float dt)
{
	if (!is_editor_mode || EditorRealtimeMode)
	{
		SystemTick(dt, physics_systems);
	}
}
