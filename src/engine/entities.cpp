#include "pch.h"

using namespace ark;

registry global_registry;
entity_view invalid_entity = {};

stl::hash_map<entt::id_type, entities::variant_type> type_map;

//stl::hash_map<physics::physics_body*, entities::physics_body_component> physics_component_storage;

void
destroy_entity(const entt::entity& ent)
{
	auto &reg = global_registry.get();
	if (reg.any_of<entities::physics_body_component>(ent)) {
		const auto& phys_body_component = reg.get<entities::physics_body_component>(ent);
		const auto body_ptr = phys_body_component.body;
		//physics_component_storage.erase(body_ptr);
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
	type_map[entt::type_id<garbage_flag>().hash()] = garbage_flag();
	type_map[entt::type_id<non_serializable_flag>().hash()] = non_serializable_flag();
	type_map[entt::type_id<dont_free_after_reset_flag>().hash()] = dont_free_after_reset_flag();
	type_map[entt::type_id<background_flag>().hash()] = background_flag();
	type_map[entt::type_id<drawable_flag>().hash()] = drawable_flag();
	type_map[entt::type_id<ground_flag>().hash()] = ground_flag();
	type_map[entt::type_id<level_flag>().hash()] = level_flag();
	type_map[entt::type_id<draw_color_component>().hash()] = draw_color_component();
	type_map[entt::type_id<draw_gradient_component>().hash()] = draw_gradient_component();
	type_map[entt::type_id<draw_texture_component>().hash()] = draw_texture_component();
	type_map[entt::type_id<scene_component>().hash()] = scene_component();
	type_map[entt::type_id<physics_body_component>().hash()] = physics_body_component();
	type_map[entt::type_id<visual_component>().hash()] = visual_component();
}                                                           

void
entities::destroy()
{
}

void
entities::tick(float dt)
{
	const auto view = global_registry.get().view<garbage_flag>();
	for (const auto ent : view) {
		destroy_entity(ent);
	}
}

void
entities::serialize(stl::stream_vector& data)
{
	const auto& reg = global_registry.get();
	reg.each([&reg, &data](entt::entity ent) {
		// #TODO: optimize
		if (reg.any_of<non_serializable_flag, garbage_flag>(ent)) {
			return;
		}
		
		serialize_desc desc = {};
		for (auto&& curr : reg.storage()) {
			entt::id_type id = curr.first;
			auto& storage = curr.second;
			if (storage.contains(ent) && type_map.contains(id)) {
				std::visit([&]<typename T>(T&& arg) {
					using U = std::remove_cv_t<std::remove_reference_t<T>>;
					if constexpr (U::is_flag) {
						desc.flags |= U::flag;
					} else {
						const U* value_ptr = static_cast<const U*>(storage.get(ent));
						if (value_ptr != nullptr && value_ptr->can_serialize_now()) {
							desc.components_count++;
						}
					}
				}, type_map[id]);
			}
		}

		if (desc.components_count == 0) {
			return;
		}
		
		stl::write_memory(data, desc);
		if (reg.all_of<net_id_flag>(ent)) {
			// #TODO:
			//return;
		}
		
		for (auto&& curr : reg.storage()) {
			entt::id_type id = curr.first;
			auto& storage = curr.second;
			if (storage.contains(ent) && type_map.contains(id)) {
				std::visit([&]<typename T>(T&& arg) {
					using U = std::remove_cv_t<std::remove_reference_t<T>>;
					if constexpr (!U::is_flag) {
						const U* value_ptr = static_cast<const U*>(storage.get(ent));
						if (value_ptr != nullptr && value_ptr->can_serialize_now()) {
							stl::write_memory(data, id);
							value_ptr->serialize(data);
						}
					}
				}, type_map[id]);
			}
		}
	});
}

void
entities::deserialize(stl::stream_vector& data)
{
}

bool
entities::is_valid(entity_view ent)
{
	return global_registry.get().valid(ent.get());
}

bool
entities::is_null(entity_view ent)
{
	return ent.get() == entt::null;
}

entity_view
entities::get_entity_from_body(const b2Body* body)
{
	const auto& registry = global_registry.get();
	const auto view = registry.view<physics_body_component>();
	for (const auto entity : view) {
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
entities::mark_as_garbage(entity_view ent)
{
	const auto& registry = global_registry.get();
	if (!registry.all_of<dont_free_after_reset_flag>(ent.get()) && !registry.all_of<garbage_flag>(ent.get())) {
		add_field<garbage_flag>(ent.get());
	}
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
entities::add_texture(entity_view ent, stl::string_view path)
{
	const auto texture_id = render::load_texture(path);
	ark_assert(texture_id != nullptr, "can't load texture", return {})

	add_field<draw_texture_component>(ent, texture_id);
	return ent;
}

entity_view
entities::add_phys_body(
	entity_view ent,
	ark_float_vec2 vel,
	ark_float_vec2 pos,
	ark_float_vec2 size,
	physics::body_type type,
	physics::body_shape shape,
	material::type mat
)
{
	auto& reg = get_registry().get();
	const physics::body_parameters phys_parameters(0.f, 0.f, vel, pos, size, type, shape, mat);
	physics::physics_body* body = schedule_creation(phys_parameters);
	
	//physics_component_storage[body] = physics_body_component(body);
	add_field<physics_body_component>(ent, body);

	return ent;
}
