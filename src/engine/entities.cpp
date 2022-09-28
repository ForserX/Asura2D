#include "pch.h"

using namespace ark;

registry global_registry;
entity_view invalid_entity = {};

stl::hash_set<entt::entity> entities_to_destroy;
stl::hash_map<physics::physics_body*, entities::physics_body_component> physics_component_storage;

void
destroy_entity(const entt::entity& ent)
{
	auto &reg = global_registry.get();
	if (reg.any_of<entities::physics_body_component>(ent)) {
		const auto& phys_body_component = reg.get<entities::physics_body_component>(ent);
		schedule_free(phys_body_component.body);
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

bool
entities::is_valid(entity_view ent)
{
	return global_registry.get().valid(ent.get());
}

entity_view
entities::get_entity_from_body(const b2Body* body)
{
	const auto& registry = global_registry.get();
	const auto& view = registry.view<physics_body_component>();
	for (auto& entity : view) {
		const auto& phys_component = registry.get<physics_body_component>(entity);
		if (phys_component.body != nullptr && phys_component.body->get_body() == body) {
			return entity;
		}
	}

	return {};
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

ark_float_vec2
entities::get_position(entity_view entity)
{
	const auto& registry = global_registry.get();
	if (registry.any_of<physics_body_component>(entity.get())) {
		const auto& phys_component = registry.get<physics_body_component>(entity.get());
		if (phys_component.body != nullptr) {
			return phys_component.body->get_position();
		}
	}
	
	return {};
}

entity_view
entities::create_phys_body(
	bool draw,
	ark_float_vec2 pos,
	ark_float_vec2 shape,
	physics::body_type type,
	material::type mat
)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();
	const physics::body_parameters phys_parameters(pos, shape, type, mat);
	physics::physics_body* body = schedule_creation(phys_parameters);
	
	physics_component_storage[body] = physics_body_component(draw, body);
	add_field<physics_body_component>(ent, physics_component_storage[body]);
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}
