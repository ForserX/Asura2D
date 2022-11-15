#include "pch.h"

using namespace ark;

void shit_detector_tick();

stl::stream_vector entities_data = {};
std::chrono::nanoseconds entities_serilaize_last_time = {};

///////////////////////////////////////////////////////////
// Helpers for serialize
///////////////////////////////////////////////////////////

auto try_to_serialize = [](std::string_view state_name)
{
	std::filesystem::path path = filesystem::get_userdata_dir();
	path.append(state_name);

	entities_data.second.clear();
	entities::internal::serialize(entities_data);
	filesystem::write_file(path, entities_data);
};

auto try_to_deserialize = [](std::string_view state_name)
{
	std::filesystem::path path = filesystem::get_userdata_dir();
	path.append(state_name);

	entities_data.second.clear();
	filesystem::read_file(path, entities_data);
	entities::internal::deserialize(entities_data);
};

///////////////////////////////////////////////////////////
// Binary deserialization
///////////////////////////////////////////////////////////
template<typename Component>
void
deserialize_entity_component(stl::stream_vector& data, entt::registry& reg, entt::entity ent, entt::id_type cmp_id)
{
	if constexpr (!stl::contains_flag_v<Component>) {
		entt::id_type id = entt::type_id<Component>().hash();
		if (id == cmp_id) {
			Component component = {};
			if constexpr (stl::is_custom_serialize_v<Component>) {
				entities::custom_serializer<Component>::deserialize(component, data);
			} else {
				stl::read_memory(data, component);
			}

			reg.emplace<Component>(ent, std::move(component));
		}
	}
}

template<typename Component>
void
deserialize_entity_flag(stl::stream_vector& data, entity_desc& desc, entt::registry& reg, entt::entity ent)
{
	if constexpr (stl::contains_flag_v<Component>) {
		if (desc.flags & Component::flag) {
			entities::add_field<Component>(ent);
		}
	}
}

template<typename... Args>
void
deserialize_entity(stl::stream_vector& data)
{
	auto& reg = entities::internal::get_registry().get();
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

///////////////////////////////////////////////////////////
// Binary serialization
///////////////////////////////////////////////////////////
template<typename Component>
void
serialize_entity_component(stl::stream_vector& data, entt::registry& reg, entt::entity ent, entity_desc& desc)
{
	if constexpr (!stl::contains_flag_v<Component>) {
		if (reg.all_of<Component>(ent)) {
			entt::id_type id = entt::type_id<Component>().hash();
			auto& storage = (*reg.storage(id)).second;

			const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
			if (value_ptr != nullptr) {
				if constexpr (stl::is_custom_serialize_v<Component>) {
					if (entities::custom_serializer<Component>::can_serialize_now(*value_ptr)) {
						stl::push_memory(data, id);
						entities::custom_serializer<Component>::serialize(*value_ptr, data);
					}
				} else {
					stl::push_memory(data, id);
					stl::push_memory(data, *value_ptr);
				}

				desc.components_count++;
			}
		}
	}
}

template<typename Component>
void
serialize_entity_flag(stl::stream_vector& data, entity_desc& desc, entt::registry& reg, entt::entity ent)
{
	if constexpr (stl::contains_flag_v<Component>) {
		if (reg.all_of<Component>(ent)) {
			desc.flags |= Component::flag;
		}
	}
}

template<typename... Args>
void
serialize_entity(stl::stream_vector& data, entt::entity ent)
{
	auto& reg = entities::internal::get_registry().get();
	if (!entities::is_valid(ent) || !reg.any_of<Args...>(ent)) {
		return;
	}

	entity_desc desc = {};
	const int64_t ent_pos = data.second.size();

	(serialize_entity_flag<Args>(data, desc, reg, ent), ...);
	stl::push_memory(data, desc);

	(serialize_entity_component<Args>(data, reg, ent, desc), ...);

	auto* desc_ptr = reinterpret_cast<entity_desc*>(&data.second[ent_pos]);
	std::memcpy(desc_ptr, &desc, sizeof(entity_desc));
}

///////////////////////////////////////////////////////////
// String serialization
///////////////////////////////////////////////////////////
template<typename Component>
void
string_serialize_entity_component(stl::string_map& data, entity_desc& desc, entt::entity ent)
{
	static_assert(
		!stl::meta::is_detected<decltype(Component::id)>::value, 
		"The type cannot contain field with \"id\" key. Try to rename it to something else"
	);

	const auto& reg = entities::internal::get_registry().get();
	if (reg.all_of<Component>(ent)) {
		if constexpr (stl::contains_flag_v<Component>) {
			desc.flags |= Component::flag;
		} else {
			entt::id_type id = entt::type_id<Component>().hash();
			auto& storage = (*reg.storage(id)).second;

			const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
			if (value_ptr != nullptr) {
				//data["id"] = std::to_string(id);
				if constexpr (stl::is_custom_serialize_v<Component>) {
					if (entities::custom_serializer<Component>::can_serialize_now(*value_ptr)) {
						entities::custom_serializer<Component>::string_serialize(*value_ptr, data);
					}
				} else {
					visit_struct::for_each(*value_ptr, []<typename T>(const char* name, const T& value) {
						static const stl::string combined_name = stl::combine_string<T>(name);
						data[combined_name] = stl::stringify(value);
					});
				}

				desc.components_count++;
			}
		}
	}
}

template<typename... Args>
void
string_serialize_entity(stl::tree_string_map& data, entt::entity ent)
{
	const auto& reg = entities::internal::get_registry().get();
	if (!entities::is_valid(ent) || !reg.any_of<Args...>(ent)) {
		return;
	}

	entity_desc desc = {};
	//std::string entity_key = std::to_string(static_cast<uint32_t>(ent)) + "" + std::to_string(desc.flags);
	//(string_serialize_entity_component<Args>(data[entity_key], desc, ent), ...);
}

///////////////////////////////////////////////////////////
// Namespace members
///////////////////////////////////////////////////////////
void
entities::deserialize_state(std::string_view state_name)
{
	scheduler::schedule(scheduler::entity_serializator, [state_name]() {
		internal::process_entities([state_name]() {
			try_to_deserialize(state_name);
		}, entities_state::writing);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void
entities::serialize_state(std::string_view state_name)
{
	scheduler::schedule(scheduler::entity_serializator, [state_name]() {
		internal::process_entities([state_name]() {
			try_to_serialize(state_name);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

const std::chrono::nanoseconds&
entities::get_last_serialize_time()
{
	return entities_serilaize_last_time;
}

void
entities::internal::serialize(stl::stream_vector& data)
{
	OPTICK_EVENT("entities serializer");
	const auto& reg = get_registry().get();
	const auto ent_view = reg.view<garbage_flag>() | reg.view<non_serializable_flag>() | reg.view<dont_free_after_reset_flag>();
	uint32_t entities_count = reg.size() - ent_view.size_hint();

	data.first = 0;
	data.second.clear();
	data.second.reserve(1 * 1024 * 1024);
	if (entities_count != 0) {
		stl::push_memory(data, entities_count);
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size()) {
			serialize_entity<DECLARE_SERIALIZABLE_ENTITY_TYPES>(data, *ent_ptr);
			ent_ptr++;
		}
	}
}

void
entities::serialize(stl::stream_vector& data)
{
	scheduler::schedule(scheduler::entity_serializator, [&data]() {
		internal::process_entities([&data]() {
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
		deserialize_entity<DECLARE_SERIALIZABLE_ENTITY_TYPES>(data);
	}
}

void
entities::deserialize(stl::stream_vector& data)
{
	scheduler::schedule(scheduler::entity_serializator, [&data]() {
		internal::process_entities([&data]() {
			internal::deserialize(data);
		}, entities_state::writing);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}
