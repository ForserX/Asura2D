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
		const auto body_ptr = phys_body_component.body;
		physics_component_storage.erase(body_ptr);
		schedule_free(body_ptr);
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

entity_view
entities::create()
{
	return global_registry.create();
}

void
entities::schedule_to_destroy(const entt::entity& ent)
{
	entities_to_destroy.insert(ent);
}

ark_float_vec2
entities::get_position(entity_view entity)
{
	const auto& registry = global_registry.get();
	if (registry.all_of<scene_component>(entity.get())) {
		const auto scene_comp= registry.try_get<scene_component>(entity.get());
		if (scene_comp != nullptr) {
			return scene_comp->position;
		}
	}
	
	if (registry.any_of<physics_body_component>(entity.get())) {
		const auto phys_comp = registry.try_get<physics_body_component>(entity.get());
		if (phys_comp != nullptr && phys_comp->body != nullptr) {
			return phys_comp->body->get_position();
		}
	}
	
	return {};
}

entity_view
entities::add_texture(
	entity_view ent,
	std::string_view path
)
{
	const auto texture_id = render::load_texture(path);
	ark_assert(texture_id != nullptr, "can't load texture", return {})

	add_field<draw_texture_component>(ent, draw_texture_component(texture_id));
	return ent;
}

entity_view
entities::add_phys_body(
	entity_view ent,
	ark_float_vec2 pos,
	ark_float_vec2 shape,
	physics::body_type type,
	material::type mat
)
{
	auto& reg = get_registry().get();
	const physics::body_parameters phys_parameters(pos, shape, type, mat);
	physics::physics_body* body = schedule_creation(phys_parameters);
	
	physics_component_storage[body] = physics_body_component(body);
	add_field<physics_body_component>(ent, physics_component_storage[body]);

	return ent;
}
