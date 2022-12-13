#include "pch.h"

using namespace Asura;

void shit_detector_tick();

stl::stream_vector entities_data = {};
stl::tree_string_map entities_string_data = {};
std::chrono::nanoseconds entities_serilaize_last_time = {};

///////////////////////////////////////////////////////////
// Helpers for serialize
///////////////////////////////////////////////////////////

auto try_to_serialize = [](stl::string_view state_name)
{
	std::filesystem::path path = FileSystem::UserdataDir();
	path.append(state_name);

	entities_data.second.clear();
	Entities::internal::serialize(entities_data);
	FileSystem::write_file(path, entities_data);
};

auto try_to_deserialize = [](stl::string_view state_name)
{
	std::filesystem::path path = FileSystem::UserdataDir();
	path.append(state_name);

	entities_data.second.clear();
	FileSystem::read_file(path, entities_data);
	Entities::internal::deserialize(entities_data);
};

auto try_to_string_serialize = [](stl::string_view state_name)
{
	std::filesystem::path path = FileSystem::UserdataDir();
	path.append(state_name);

	entities_string_data.clear();
	Entities::internal::string_serialize(entities_string_data);

	CfgParser parser;
	parser.swap(entities_string_data);
	parser.save(path);
};

auto try_to_string_deserialize = [](stl::string_view state_name)
{
	std::filesystem::path path = FileSystem::UserdataDir();
	path.append(state_name);

	entities_string_data.clear();
	CfgParser parser;
	parser.load(path);

	Entities::internal::string_deserialize(parser.get_data());
};


///////////////////////////////////////////////////////////
// Binary deserialization
///////////////////////////////////////////////////////////
template<typename Component>
void deserialize_entity_component(stl::stream_vector& data, entt::registry& reg, entt::entity ent, entt::id_type cmp_id)
{
	if constexpr (!stl::contains_flag_v<Component>) 
	{
		entt::id_type id = entt::type_id<Component>().hash();
		if (id == cmp_id) {
			Component component = {};
			if constexpr (stl::is_custom_serialize_v<Component>) 
			{
				Entities::custom_serializer<Component>::deserialize(component, data);
			} 
			else 
			{
				stl::read_memory(data, component);
			}

			reg.emplace<Component>(ent, std::move(component));
		}
	}
}

template<typename Component>
void deserialize_entity_flag(stl::stream_vector& data, entity_desc& desc, entt::registry& reg, entt::entity ent)
{
	if constexpr (stl::contains_flag_v<Component>)
	{
		if (desc.flags & Component::flag)
		{
			Entities::add_field<Component>(ent);
		}
	}
}

template<typename... Args>
void deserialize_entity(stl::stream_vector& data)
{
	auto& reg = Entities::internal::get_registry().Get();
	entity_desc desc = {};
	stl::read_memory(data, desc);

	EntityView ent = Entities::Create();
	(deserialize_entity_flag<Args>(data, desc, reg, ent.Get()), ...);

	for (int32_t i = 0; i < desc.components_count; i++)
	{
		entt::id_type component_type = 0;
		stl::read_memory(data, component_type);
		(deserialize_entity_component<Args>(data, reg, ent.Get(), component_type), ...);
	}
}

///////////////////////////////////////////////////////////
// Binary serialization
///////////////////////////////////////////////////////////
template<typename Component>
void serialize_entity_component(stl::stream_vector& data, entt::registry& reg, entt::entity ent, entity_desc& desc)
{
	if constexpr (!stl::contains_flag_v<Component>) 
	{
		if (reg.all_of<Component>(ent))
		{
			entt::id_type id = entt::type_id<Component>().hash();
			auto& storage = (*reg.storage(id)).second;

			const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
			if (value_ptr != nullptr) 
			{
				if constexpr (stl::is_custom_serialize_v<Component>) 
				{
					if (Entities::custom_serializer<Component>::can_serialize_now(*value_ptr)) 
					{
						stl::push_memory(data, id);
						Entities::custom_serializer<Component>::serialize(*value_ptr, data);
					}
				} 
				else
				{
					stl::push_memory(data, id);
					stl::push_memory(data, *value_ptr);
				}

				desc.components_count++;
			}
		}
	}
}

template<typename Component>
void serialize_entity_flag(stl::stream_vector& data, entity_desc& desc, entt::registry& reg, entt::entity ent)
{
	if constexpr (stl::contains_flag_v<Component>) 
	{
		if (reg.all_of<Component>(ent)) {
			desc.flags |= Component::flag;
		}
	}
}

template<typename... Args>
void serialize_entity(stl::stream_vector& data, entt::entity ent)
{
	auto& reg = Entities::internal::get_registry().Get();
	if (!Entities::IsValid(ent) || !reg.any_of<Args...>(ent)) 
	{
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
void string_serialize_entity_component(stl::string_map& data, entity_desc& desc, stl::vector<entt::id_type>& components, entt::entity ent)
{
	const auto& reg = Entities::internal::get_registry().Get();

	if (reg.all_of<Component>(ent)) 
	{
		if constexpr (stl::contains_flag_v<Component>) 
		{
			desc.flags |= Component::flag;
		} 
		else 
		{
			entt::id_type id = entt::type_id<Component>().hash();
			auto& storage = (*reg.storage(id)).second;

			const Component* value_ptr = static_cast<const Component*>(storage.get(ent));
			if (value_ptr != nullptr) 
			{
				components.push_back(id);
				if constexpr (stl::is_custom_serialize_v<Component>) 
				{
					if (Entities::custom_serializer<Component>::can_serialize_now(*value_ptr)) 
					{
						Entities::custom_serializer<Component>::string_serialize(*value_ptr, data);
					}
				}
				else 
				{
					visit_struct::for_each(*value_ptr, [&data]<typename T>(const char* name, const T& value) 
					{
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
void string_serialize_entity(stl::tree_string_map& data, entt::entity ent)
{
	const auto& reg = Entities::internal::get_registry().Get();
	if (!Entities::IsValid(ent) || !reg.any_of<Args...>(ent)) 
	{
		return;
	}

	entity_desc desc = {};
	stl::vector<entt::id_type> components;

	stl::string entity_key = stl::to_string(static_cast<uint32_t>(ent));
	stl::string_map& ent_data = data[entity_key];

	(string_serialize_entity_component<Args>(ent_data, desc, components, ent), ...);

	if (desc.flags != 0) 
	{
		ent_data["i_flags"] = stl::to_string(desc.flags);
	}

	if (!components.empty()) 
	{
		ent_data["arr_components"] = stl::stringify(components);
	}

}

///////////////////////////////////////////////////////////
// Namespace members
///////////////////////////////////////////////////////////
void Entities::deserialize_state(stl::string_view state_name)
{
	Scheduler::schedule(Scheduler::entity_serializator, [state_name]() 
		{
		internal::process_entities([state_name]() {
			try_to_deserialize(state_name);
		}, entities_state::writing);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void Entities::serialize_state(stl::string_view state_name)
{
	Scheduler::schedule(Scheduler::entity_serializator, [state_name]()
	{
		internal::process_entities([state_name]()
			{
			try_to_serialize(state_name);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void Entities::string_serialize_state(stl::string_view state_name)
{
	Scheduler::schedule(Scheduler::entity_serializator, [state_name]()
	{
		internal::process_entities([state_name]() 
		{
			try_to_string_serialize(state_name);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void Entities::string_deserialize_state(stl::string_view state_name)
{
	Scheduler::schedule(Scheduler::entity_serializator, [state_name]()
	{
		internal::process_entities([state_name]()
		{
			try_to_string_deserialize(state_name);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

const std::chrono::nanoseconds& Entities::get_last_serialize_time()
{
	return entities_serilaize_last_time;
}

void Entities::internal::serialize(stl::stream_vector& data)
{
	OPTICK_EVENT("Entities serializer");
	const auto& reg = get_registry().Get();
	const auto ent_view = reg.view<garbage_flag>() | reg.view<non_serializable_flag>() | reg.view<dont_free_after_reset_flag>();
	uint32_t entities_count = reg.size() - ent_view.size_hint();

	data.first = 0;
	data.second.clear();
	data.second.reserve(1 * 1024 * 1024);

	if (entities_count != 0) 
	{
		stl::push_memory(data, entities_count);
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size())
		{
			serialize_entity<DECLARE_SERIALIZABLE_ENTITY_TYPES>(data, *ent_ptr);
			ent_ptr++;
		}
	}
}

void Entities::serialize(stl::stream_vector& data)
{
	Scheduler::schedule(Scheduler::entity_serializator, [&data]()
	{
		internal::process_entities([&data]() 
		{
			internal::serialize(data);
		}, entities_state::reading);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void Entities::internal::deserialize(stl::stream_vector& data)
{
	OPTICK_EVENT("Entities deserializer");

	free();
	clear();
	shit_detector_tick();
	data.first = 0;

	uint32_t entities_count = 0;
	stl::read_memory(data, entities_count);

	for (uint32_t i = 0; i < entities_count; i++) 
	{
		deserialize_entity<DECLARE_SERIALIZABLE_ENTITY_TYPES>(data);
	}
}

void Entities::internal::string_serialize(stl::tree_string_map& data)
{
	OPTICK_EVENT("Entities string serializer");

	const auto& reg = get_registry().Get();
	const auto ent_view = reg.view<garbage_flag>() | reg.view<non_serializable_flag>() | reg.view<dont_free_after_reset_flag>();
	uint32_t entities_count = reg.size() - ent_view.size_hint();

	if (entities_count != 0)
	{
		const entt::entity* ent_ptr = reg.data();
		while (ent_ptr != reg.data() + reg.size()) 
		{
			string_serialize_entity<DECLARE_SERIALIZABLE_ENTITY_TYPES>(data, *ent_ptr);
			ent_ptr++;
		}
	}
}

void Entities::internal::string_deserialize(const stl::tree_string_map& data)
{
	OPTICK_EVENT("Entities string deserializer");
}

void Entities::deserialize(stl::stream_vector& data)
{
	Scheduler::schedule(Scheduler::entity_serializator, [&data]() 
	{
		internal::process_entities([&data]() 
		{
			internal::deserialize(data);
		}, entities_state::writing);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void Entities::string_serialize(stl::tree_string_map& data)
{
	Scheduler::schedule(Scheduler::entity_serializator, [&data]() 
	{
		internal::process_entities([&data]()
		{
			internal::string_serialize(data);
		}, entities_state::writing);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}

void Entities::string_deserialize(const stl::tree_string_map& data)
{
	Scheduler::schedule(Scheduler::entity_serializator, [&data]() 
	{
		internal::process_entities([&data]() 
		{
			internal::string_deserialize(data);
		}, entities_state::writing);

		entities_serilaize_last_time = std::chrono::steady_clock::now().time_since_epoch();
		return false;
	});
}
