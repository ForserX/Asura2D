#include "pch.h"

using namespace ark;

bool is_started = false;

std::set<ark::system*> pre_update_systems;
std::set<ark::system*> update_systems;
std::set<ark::system*> post_update_systems;

registry global_registry;

std::set<ark::system*>*
get_system_by_type(systems::update_type type)
{
	switch (type) {
	case systems::update_type::pre_update_schedule:
		return &pre_update_systems;
	case systems::update_type::update_schedule:
		return &update_systems;
	case systems::update_type::post_update_schedule:
		return &post_update_systems;
	}

	return nullptr;
}

void
systems::delete_system(system* system_to_delete, update_type type)
{
	std::set<system*>* systems_list = get_system_by_type(type);
	ark_assert(systems_list->contains(system_to_delete), "Pointer not found", std::terminate());
	systems_list->erase(system_to_delete);
}

void
systems::add_system(system* system_to_add, update_type type)
{
	std::set<system*>* systems_list = get_system_by_type(type);
	ark_assert(!systems_list->contains(system_to_add), "Pointer is alive! Duplicate!", std::terminate());
	systems_list->insert(system_to_add);

	if (is_started) {
		system_to_add->init();
	}
}

void
systems::init()
{
	for	(const auto system : pre_update_systems) {
		system->init();
	}
	
	for	(const auto system : update_systems) {
		system->init();
	}

	for	(const auto system : post_update_systems) {
		system->init();
	}

	is_started = true;
}

void
systems::destroy()
{
	pre_update_systems.clear();
	update_systems.clear();
	post_update_systems.clear();
}

void
systems::tick(float dt)
{
	for	(const auto system : pre_update_systems) {
		system->tick(global_registry, dt);
	}
	
	for	(const auto system : update_systems) {
		system->tick(global_registry, dt);
	}

	for	(const auto system : post_update_systems) {
		system->tick(global_registry, dt);
	}
}
