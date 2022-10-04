#include "pch.h"

using namespace ark;

registry global_registry;
entity_view invalid_entity = {};

std::chrono::nanoseconds entities_serilize_last_time;
stl::stream_vector entities_data;
marl::mutex entities_serialization_lock;

input::on_key_change entities_key_change_event;

void
destroy_entity(const entt::entity& ent)
{
	auto &reg = global_registry.get();
	if (auto phys_comp = reg.try_get<entities::physics_body_component>(ent)) {
		physics::schedule_free(phys_comp->body);
	}

	reg.destroy(ent);
}

registry&
entities::get_registry()
{
	return global_registry;
}

auto try_to_serialize = []()
{
	std::filesystem::path path = filesystem::get_userdata_dir();
	path.append("game_state");

	entities_data.second.resize(0);
	entities::serialize(entities_data);
	filesystem::write_file(path, entities_data);
};

auto try_to_deserialize = []()
{
	std::filesystem::path path = filesystem::get_userdata_dir();
	path.append("game_state");

	entities_data.second.resize(0);
	filesystem::read_file(path, entities_data);
	entities::deserialize(entities_data);
};

void
entities::init()
{
#ifndef ARKANE_SHIPPING
	entities_key_change_event = input::subscribe_key_event([](int16_t scan_code, input::key_state state)  
	{	
		if (state == input::key_state::press) {
			switch (scan_code) {
				case SDL_SCANCODE_F6: {
					try_to_serialize();
					break;
				}
				case SDL_SCANCODE_F8: {
					try_to_deserialize();
					break;
				}
			}
		}
	});
#endif
}                                                           

void
entities::destroy()
{
#ifndef ARKANE_SHIPPING
	input::unsubscribe_key_event(entities_key_change_event);
#endif
}

void
entities::tick(float dt)
{
	const auto view = global_registry.get().view<garbage_flag>();
	for (auto ent : view) {
		destroy_entity(ent);
	}
}

template<typename Component>
void serialize_entity_component(stl::stream_vector& data, entt::entity ent, entity_desc& desc)
{
	const auto& reg = global_registry.get();
	if (reg.all_of<Component>(ent)) {
		entt::id_type id = entt::type_id<Component>().hash();
		auto& storage = (*reg.storage(id)).second;
		if constexpr (entities::is_flag_v<Component>) {
			desc.flags |= Component::flag;
		} else {
			const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
			if (value_ptr != nullptr && value_ptr->can_serialize_now()) {
				stl::push_memory(data, id);
				value_ptr->serialize(data);
				desc.components_count++;
			}
		}
	}
}

template<typename... Args>
void serialize_entity(stl::stream_vector& data, entt::entity ent)
{
	const auto& reg = global_registry.get();
	if (!entities::is_valid(ent) || !reg.any_of<Args...>(ent)) {
		return;
	}

	entity_desc desc = {};
	const int64_t ent_pos = data.first;

	stl::push_memory(data, desc);
	(serialize_entity_component<Args>(data, ent, desc), ...);

	auto* desc_ptr = reinterpret_cast<entity_desc*>(&data.second[ent_pos]);
	std::memcpy(desc_ptr, &desc, sizeof(entity_desc));
}

template<typename Component>
void string_serialize_entity_component(stl::string_map& data, entt::entity ent, entity_desc& desc)
{
	const auto& reg = global_registry.get();
	if (reg.all_of<Component>(ent)) {
		entt::id_type id = entt::type_id<Component>().hash();
		auto& storage = (*reg.storage(id)).second;
		if constexpr (entities::is_flag_v<Component>) {
			desc.flags |= Component::flag;
		} else {
			const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
			if (value_ptr != nullptr && value_ptr->can_serialize_now()) {
				value_ptr->string_serialize(data);
				desc.components_count++;
			}
		}
	}
}

template<typename... Args>
void string_serialize_entity(stl::tree_string_map& data, entt::entity ent)
{
	const auto& reg = global_registry.get();
	if (!entities::is_valid(ent) || !reg.any_of<Args...>(ent)) {
		return;
	}

	entity_desc desc = {};
	stl::string entity_key = std::to_string(static_cast<uint32_t>(ent));
	(string_serialize_entity_component<Args>(data[entity_key], ent, desc), ...);
	data[entity_key]["flags"] = std::to_string(desc.flags);
}

void
entities::free()
{
	const auto& reg = global_registry.get();
	const entt::entity* ent_ptr = reg.data();
	while (ent_ptr != reg.data() + reg.size()) {
		entities::mark_as_garbage(*ent_ptr);
		ent_ptr++;
	}
}

std::chrono::nanoseconds&
entities::get_last_serialize_time()
{
	return entities_serilize_last_time;
}

void 
entities::string_serialize(stl::tree_string_map& data)
{
	marl::lock scope_lock(entities_serialization_lock);
	while (is_phys_ticking != false && is_game_ticking != false) {
		threads::switch_context();
	}

	is_serializer_ticking = true;
	OPTICK_EVENT("entities serializer")
	const auto& reg = global_registry.get();
	const auto ent_view = reg.view<garbage_flag>() | reg.view<non_serializable_flag>() | reg.view<dont_free_after_reset_flag>();
	const uint32_t entities_count = reg.size() - ent_view.size_hint();

	if (entities_count != 0) {
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size()) {
			string_serialize_entity<DECLARE_SERIALIZABLE_TYPES>(data, *ent_ptr);
			ent_ptr++;
		}
	}

	entities_serilize_last_time = std::chrono::steady_clock::now().time_since_epoch();
	is_serializer_ticking = false;
}

void 
entities::string_deserialize(const stl::tree_string_map& data)
{

}

void
entities::serialize(stl::stream_vector& data)
{
	marl::lock scope_lock(entities_serialization_lock);
	while (is_phys_ticking != false && is_game_ticking != false) {
		threads::switch_context();
	}

	is_serializer_ticking = true;
	OPTICK_EVENT("entities serializer")
	const auto& reg = global_registry.get();
	const auto ent_view = reg.view<garbage_flag>() | reg.view<non_serializable_flag>() | reg.view<dont_free_after_reset_flag>();
	const uint32_t entities_count = reg.size() - ent_view.size_hint();

	data.second.reserve(300 * 1024 * 1024);
	if (entities_count != 0) {
		stl::push_memory(data, entities_count);
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size()) {
			serialize_entity<DECLARE_SERIALIZABLE_TYPES>(data, *ent_ptr);
			ent_ptr++;
		}
	}

	entities_serilize_last_time = std::chrono::steady_clock::now().time_since_epoch();
	is_serializer_ticking = false;
}

void
entities::deserialize(stl::stream_vector& data)
{
	marl::lock scope_lock(entities_serialization_lock);
	while (is_phys_ticking != false && is_game_ticking != false) {
		threads::switch_context();
	}

	is_serializer_ticking = true;
	OPTICK_EVENT("entities deserializer")
	data.first = 0;
	data.second.clear();

	uint32_t entities_count = 0;
	stl::read_memory(data, entities_count);
	for (uint32_t i = 0; i < entities_count; i++) {

	}

	entities_serilize_last_time = std::chrono::steady_clock::now().time_since_epoch();
	is_serializer_ticking = false;
}

bool
entities::is_valid(entity_view ent)
{
	return !is_null(ent) && ent.get() != entt::tombstone && global_registry.get().valid(ent.get());
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
	material::shape shape,
	material::type mat
)
{
	auto& reg = get_registry().get();
	const physics::body_parameters phys_parameters(0.f, 0.f, vel, pos, size, type, shape, mat);
	physics::physics_body* body = schedule_creation(phys_parameters);
	
	add_field<physics_body_component>(ent, body);
	if (!reg.all_of<scene_component>(ent.get())) {
		add_field<scene_component>(ent);
	}

	return ent;
}
