#include "pch.h"

using namespace ark;

registry global_registry;

registry&
entities::get_registry()
{
	return global_registry;
}

entt::entity
entities::create_entity()
{
	return global_registry.create();
}

void
entities::destroy_entity(const entt::entity& ent)
{
	global_registry.destroy(ent);
}

entity
entities::create_phys_ground_entity(bool draw, b2Vec2 pos, b2Vec2 shape)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	b2Body* body = physics::create_static(pos, shape);
	physics_body_component phys_body = {draw, body};
	add_field<physics_body_component>(ent, phys_body);
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}

entity
entities::create_phys_body_entity(bool draw, b2Vec2 pos, b2Vec2 shape)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	physics_body_component phys_body = {};
	phys_body.body = physics::create_dynamic(pos, shape);
	phys_body.is_drawing = draw;

	add_field<physics_body_component>(ent, phys_body);
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}

entity
entities::create_phys_body_entity_cricle(bool draw, b2Vec2 pos, b2Vec2 shape)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	physics_body_component phys_body = {};
	phys_body.body = physics::create_dynamic_cricle(pos, shape);
	phys_body.is_drawing = draw;

	add_field<physics_body_component>(ent, phys_body);
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}