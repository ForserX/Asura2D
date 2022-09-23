#include "pch.h"

using namespace ark;

registry global_registry;

std::set<entt::entity> entities_to_destroy;
std::unordered_map<physics::physics_body*, entities::physics_body_component> physics_component_storage;

void
destroy_entity(const entt::entity& ent)
{
	auto &reg = global_registry.get();
	if (reg.any_of<entities::physics_body_component>(ent)) {
		const auto& phys_body_component = reg.get<entities::physics_body_component>(ent);
		physics::schedule_free(phys_body_component.body);
	}
	
	global_registry.destroy(ent);
}

registry&
entities::get_registry()
{
	return global_registry;
}

void
entities::init()
{
}

void
entities::destroy()
{
}

void
entities::tick(float dt)
{
	for (const auto& ent : entities_to_destroy) {
		destroy_entity(ent);
	}
	
	entities_to_destroy.clear();
}

entt::entity
entities::create_entity()
{
	return global_registry.create();
}

void
entities::schedule_to_destroy_entity(const entt::entity& ent)
{
	entities_to_destroy.insert(ent);
}

entity
entities::create_phys_ground_entity(bool draw, ark_float_vec2 pos, ark_float_vec2 shape, physics::material::material_type mat)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	const physics::body_parameters phys_parameters = physics::body_parameters(pos, shape, physics::body_type::static_body, mat);

	const auto body = schedule_creation(phys_parameters);
	physics_component_storage[body] = physics_body_component(draw, body);
	add_field<physics_body_component>(ent, physics_component_storage[body]);

	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}

entity
entities::create_phys_body_entity(bool draw, ark_float_vec2 pos, ark_float_vec2 shape, physics::material::material_type mat)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	const physics::body_parameters phys_parameters = physics::body_parameters(pos, shape, physics::body_type::dynamic_body, mat);

	const auto body = schedule_creation(phys_parameters);
	physics_component_storage[body] = physics_body_component(draw, body);
	add_field<physics_body_component>(ent, physics_component_storage[body]);
	
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}

entity
entities::create_phys_body_entity_circle(bool draw, ark_float_vec2 pos, ark_float_vec2 shape, physics::material::material_type mat)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	const physics::body_parameters phys_parameters = physics::body_parameters(pos, shape, physics::body_type::around_body, mat);

	const auto body = schedule_creation(phys_parameters);
	physics_component_storage[body] = physics_body_component(draw, body);
	add_field<physics_body_component>(ent, physics_component_storage[body]);
	
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}