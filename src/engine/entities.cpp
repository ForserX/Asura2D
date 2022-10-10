#include "pch.h"

using namespace ark;

registry global_registry = {};
entity_view invalid_entity = {};

std::chrono::nanoseconds entities_serilaize_last_time = {};
stl::stream_vector entities_data = {};

input::on_key_change entities_key_change_event = {};
bool clear_on_next_tick = false;
bool free_on_next_tick = false;

void process_entities(auto&& func, uint8_t state)
{
	using namespace std::chrono_literals;
	while (serialization_state != entities_state::idle) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		//threads::switch_context();
	}

	serialization_state = state;
	func();
	serialization_state = entities_state::idle;
}

registry&
entities::internal::get_registry()
{
	return global_registry;
}

auto try_to_serialize = [](std::string_view state_name)
{
	std::filesystem::path path = filesystem::get_userdata_dir();
	path.append(state_name);

	entities_data.second.resize(0);
	entities::internal::serialize(entities_data);
	filesystem::write_file(path, entities_data);
};

auto try_to_deserialize = [](std::string_view state_name)
{
	std::filesystem::path path = filesystem::get_userdata_dir();
	path.append(state_name);

	entities_data.second.resize(0);
	filesystem::read_file(path, entities_data);
	entities::deserialize(entities_data);
};

auto shit_detector_tick = []() 
{
	auto& reg = global_registry.get();
	auto destroy_ent = [&reg](entt::entity ent)
	{
		if (entities::is_valid(ent)) {
			if (auto phys_comp = reg.try_get<entities::physics_body_component>(ent)) {
				physics::schedule_free(phys_comp->body);
			}

			reg.destroy(ent);
		}
	};

	if (free_on_next_tick) {
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size()) {	
			entt::entity ent = *ent_ptr;
			if (entities::is_valid(ent) && !entities::contains<entities::garbage_flag>(ent)) {
				entities::add_field<entities::garbage_flag>(ent);
			}

			ent_ptr++;
		}

		free_on_next_tick = false;
	}
	
	const auto view = reg.view<entities::garbage_flag>();
	view.each([&reg, &destroy_ent](entt::entity ent) {
		destroy_ent(ent);
	});

	if (view.size() == reg.size() && clear_on_next_tick) {
		reg.clear();
		clear_on_next_tick = false;
	}
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
					entities::serialize_state("game_state");
					break;
				}
				case SDL_SCANCODE_F8: {
					entities::deserialize_state("game_state");
					break;
				}
			}
		}
	});
#endif

	scheduler::schedule(scheduler::garbage_collector, []() {
		process_entities([]() {
			shit_detector_tick();
		}, entities_state::cleaning_up);

		return true;
	});
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

}

template<typename Component>
void 
string_serialize_entity_component(stl::string_map& data, entt::entity ent, entity_desc& desc)
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
void 
string_serialize_entity(stl::tree_string_map& data, entt::entity ent)
{
	const auto& reg = global_registry.get();
	if (!entities::is_valid(ent) || !reg.any_of<Args...>(ent)) {
		return;
	}

	entity_desc desc = {};
	std::string entity_key = std::to_string(static_cast<uint32_t>(ent));
	(string_serialize_entity_component<Args>(data[entity_key], ent, desc), ...);
	data[entity_key]["flags"] = std::to_string(desc.flags);
}

template<typename Component>
void 
serialize_entity_component(stl::stream_vector& data, entt::registry& reg, entt::entity ent, entity_desc& desc)
{
	if constexpr (!entities::is_flag_v<Component>) {
		if (reg.all_of<Component>(ent)) {
			entt::id_type id = entt::type_id<Component>().hash();
			auto& storage = (*reg.storage(id)).second;
			const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
			if (value_ptr != nullptr && value_ptr->can_serialize_now()) {
				stl::push_memory(data, id);
				value_ptr->serialize(data);
				desc.components_count++;
			}
		}
	}
}

template<typename Component>
void
deserialize_entity_component(stl::stream_vector& data, entt::registry& reg, entt::entity ent, entt::id_type cmp_id)
{
	if constexpr (!entities::is_flag_v<Component>) {
		entt::id_type id = entt::type_id<Component>().hash();
		if (id == cmp_id) {
			Component component = {};
			component.deserialize(data);
			reg.emplace<Component>(ent, std::move(component));
		}
	}
}

template<typename Component>
void
serialize_entity_flag(stl::stream_vector& data, entt::registry& reg, entt::entity ent, entity_desc& desc)
{
	if constexpr (entities::is_flag_v<Component>) {
		if (reg.all_of<Component>(ent)) {
			entt::id_type id = entt::type_id<Component>().hash();
			auto& storage = (*reg.storage(id)).second;
			desc.flags |= Component::flag;
		}
	}
}

template<typename Component>
void
deserialize_entity_flag(stl::stream_vector& data, entity_desc& desc, entt::registry& reg, entt::entity ent)
{
	if constexpr (entities::is_flag_v<Component>) {
		if (desc.flags & Component::flag) {
			entities::add_field<Component>(ent);
		}
	}
}

template<typename... Args>
void 
serialize_entity(stl::stream_vector& data, entt::entity ent)
{
	auto& reg = global_registry.get();
	if (!entities::is_valid(ent) || !reg.any_of<Args...>(ent)) {
		return;
	}
	
	entity_desc desc = {};
	const int64_t ent_pos = data.second.size();
	stl::push_memory(data, desc);

	(serialize_entity_component<Args>(data, reg, ent, desc), ...);
	(serialize_entity_flag<Args>(data, reg, ent, desc), ...);

	auto* desc_ptr = reinterpret_cast<entity_desc*>(&data.second[ent_pos]);
	std::memcpy(desc_ptr, &desc, sizeof(entity_desc));
}

template<typename... Args>
void 
deserialize_entity(stl::stream_vector& data)
{
	auto& reg = global_registry.get();
	entity_desc desc = {};
	stl::read_memory(data, desc);
	
	entity_view ent = entities::create();
	(deserialize_entity_flag<Args>(data, desc, reg, ent.get()), ...);

	for (int32_t i = 0; i < desc.components_count; i++) {
		entt::id_type component_type = 0;
		stl::read_memory(data, component_type);
		(deserialize_entity_component<Args>(data, reg, ent.get(), component_type), ...);
	}
}

void 
entities::deserialize_state(std::string_view state_name)
{
	scheduler::schedule(scheduler::entity_serializator, [state_name]() {
		process_entities([state_name]() {
			try_to_deserialize(state_name);
			}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void 
entities::serialize_state(std::string_view state_name)
{
	scheduler::schedule(scheduler::entity_serializator, [state_name]() {
		process_entities([state_name]() {
			try_to_serialize(state_name);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void 
entities::clear()
{
	clear_on_next_tick = true;
}

void
entities::free()
{
	free_on_next_tick = true;
}

std::chrono::nanoseconds&
entities::get_last_serialize_time()
{
	return entities_serilaize_last_time;
}

void
entities::internal::string_serialize(stl::tree_string_map& data)
{
	OPTICK_EVENT("entities serializer");
	const auto& reg = global_registry.get();
	const auto ent_view = reg.view<garbage_flag>() | reg.view<non_serializable_flag>() | reg.view<dont_free_after_reset_flag>();
	const uint32_t entities_count = reg.size() - ent_view.size_hint();

	if (entities_count != 0) {
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size()) {
			string_serialize_entity<DECLARE_ENTITIES_TYPES>(data, *ent_ptr);
			ent_ptr++;
		}
	}
}

void 
entities::string_serialize(stl::tree_string_map& data)
{
	scheduler::schedule(scheduler::entity_serializator, [&data]() {
		process_entities([&data]() {
			internal::string_serialize(data);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void
entities::internal::string_deserialize(const stl::tree_string_map& data)
{

}

void 
entities::string_deserialize(const stl::tree_string_map& data)
{

}

void
entities::internal::serialize(stl::stream_vector& data)
{
	OPTICK_EVENT("entities serializer");
	const auto& reg = global_registry.get();
	const auto ent_view = reg.view<garbage_flag>() | reg.view<non_serializable_flag>() | reg.view<dont_free_after_reset_flag>();
	volatile uint32_t entities_count = reg.size() - ent_view.size_hint();

	data.first = 0;
	data.second.clear();
	data.second.reserve(1 * 1024 * 1024);
	if (entities_count != 0) {
		stl::push_memory(data, entities_count);
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size()) {
			serialize_entity<DECLARE_ENTITIES_TYPES>(data, *ent_ptr);
			ent_ptr++;
		}
	}
}

void
entities::serialize(stl::stream_vector& data)
{
	scheduler::schedule(scheduler::entity_serializator, [&data]() {
		process_entities([&data]() {
			internal::serialize(data);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void
entities::internal::deserialize(stl::stream_vector& data)
{
	OPTICK_EVENT("entities deserializer");

	free();
	clear();
	shit_detector_tick();
	data.first = 0;

	uint32_t entities_count = 0;
	stl::read_memory(data, entities_count);
	for (uint32_t i = 0; i < entities_count; i++) {
		deserialize_entity<DECLARE_ENTITIES_TYPES>(data);
	}
}

void
entities::deserialize(stl::stream_vector& data)
{
	scheduler::schedule(scheduler::entity_serializator, [&data]() {
		process_entities([&data]() {
			internal::deserialize(data);
		}, entities_state::writing);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
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

ark_float_vec2
entities::get_position(entity_view entity)
{
	if (contains<scene_component>(entity)) {
		const auto scene_comp = try_get<scene_component>(entity.get());
		if (scene_comp != nullptr) {
			return scene_comp->position;
		}
	}

	return {};
}

entity_view
entities::get_entity_from_body(const b2Body* body)
{
	const auto view = get_view<physics_body_component>();
	for (const auto entity : view) {
		const auto phys_component = try_get<physics_body_component>(entity);
		if (phys_component != nullptr && phys_component->body != nullptr && phys_component->body->get_body() == body) {
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

entity_view
entities::add_texture(entity_view ent, stl::string_view path)
{
    const resources::id_type texture_resource = resources::load(path);
	const ImTextureID texture_id = render::load_texture(texture_resource);
	ark_assert(texture_id != nullptr, "can't load texture", return ent)

	add_field<draw_texture_component>(ent, texture_resource);
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
	const physics::body_parameters phys_parameters(0.f, 0.f, vel, pos, size, type, shape, mat);
	physics::physics_body* body = schedule_creation(phys_parameters);
	
	add_field<physics_body_component>(ent, body);
	if (!contains<scene_component>(ent.get())) {
		add_field<scene_component>(ent);
	}

	return ent;
}
