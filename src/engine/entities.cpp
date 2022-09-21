#include "pch.h"

using namespace ark;

registry global_registry;

std::set<entt::entity> entities_to_destroy;

void
destroy_entity(const entt::entity& ent)
{
	auto &reg = global_registry.get();
	
	if (reg.any_of<entities::physics_body_component>(ent)) {
		const auto &entity = reg.get<entities::physics_body_component>(ent);
		physics::destroy_body(entity.body);
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
entities::create_phys_ground_entity(bool draw, b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	b2Body* body = physics::create_static(pos, shape, mat);
	physics_body_component phys_body = {draw, body};
	add_field<physics_body_component>(ent, phys_body);
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}

entity
entities::create_phys_body_entity(bool draw, b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	physics_body_component phys_body = {};
	phys_body.body = physics::create_dynamic(pos, shape, mat);
	phys_body.is_drawing = draw;

	add_field<physics_body_component>(ent, phys_body);
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}

entity
entities::create_phys_body_entity_cricle(bool draw, b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat)
{
	auto& reg = get_registry().get();
	const entt::entity ent = create_entity();

	physics_body_component phys_body = {};
	phys_body.body = physics::create_dynamic_cricle(pos, shape, mat);
	phys_body.is_drawing = draw;

	add_field<physics_body_component>(ent, phys_body);
	if (draw) {
		add_field<drawable_flag>(ent);
	}
	
	return ent;
}