#pragma once

namespace ark
{
	class registry
	{
	private:
		entt::registry base_registry;

	public:
		entt::registry& get() { return base_registry; }
		
		entt::entity create() { return base_registry.create(); }
		void destroy(const entt::entity& entity) { base_registry.destroy(entity); }
	};
	
	class entity_view
	{
	private:
		entt::entity ent;
		
	public:
		entity_view(entt::entity in_ent) : ent(in_ent) {}
		entity_view(entity_view const&) = default;
		entity_view() = default;
		~entity_view() = default;

		const entt::entity& get() const { return ent; }
	};

	struct entity_desc
	{
		uint16_t flags;
		uint8_t components_count;
		uint8_t reserved2;
	};
}

namespace ark::entities
{
	namespace internal
	{
		registry& get_registry();

		void string_serialize(stl::tree_string_map& data);
		void string_deserialize(const stl::tree_string_map& data);

		void serialize(stl::stream_vector& data);
		void deserialize(stl::stream_vector& data);
	}

	template<typename... Args>
	void access(auto&& func, Args&&...args)
	{
		{
			OPTICK_EVENT("waiting for access")
			uint8_t state = serialization_state.load();
			while (state != entities_state::idle && state != entities_state::viewing) {
				state = serialization_state.load();
				threads::switch_context();
			}
		}

		if (serialization_state == entities_state::idle) {
			serialization_state = entities_state::viewing;
		}

		serialization_ref_counter++;
		func(std::forward<Args>(args)...);
		serialization_ref_counter--;

		if (serialization_ref_counter == 0) {
			serialization_state = entities_state::idle;
		}
	}

	template<typename Type, typename... Args>
	void add_field(const entt::entity& entt, Args &&...args)
	{
		internal::get_registry().get().emplace<Type>(entt, std::forward<Args>(args)...);
	}

	template<typename Type, typename... Args>
	void add_field(const entity_view& entt, Args &&...args)
	{
		add_field<Type, Args...>(entt.get(), std::forward<Args>(args)...);
	}

	template<typename Type>
	void erase_field(const entt::entity& entt)
	{
		internal::get_registry().get().remove<Type>(entt);
	}

	template<typename Type>
	void erase_field(const entity_view& entt)
	{
		erase_field<Type>(entt.get());
	}

	template<typename... Args>
	auto get_view()
	{
		auto view = internal::get_registry().get().view<Args...>(entt::exclude<garbage_flag>);
		return view;
	}

	template<typename Type>
	auto try_get(const entt::entity ent)
	{
		return internal::get_registry().get().try_get<Type>(ent);
	}

	template<typename Type>
	auto try_get(const entity_view& ent)
	{
		return try_get<Type>(ent.get());
	}


	template<typename Type>
	auto get(entt::entity ent)
	{
		return internal::get_registry().get().get<Type>(ent);
	}

	template<typename Type>
	auto get(const entity_view& ent)
	{
		return get<Type>(ent.get());
	}

	template<typename... Args>
	bool contains(const entt::entity ent)
	{
		return internal::get_registry().get().all_of<Args...>(ent);
	}

	template<typename... Args>
	bool contains(const entity_view& ent)
	{
		return contains<Args...>(ent.get());
	}

	template<typename... Args>
	bool contains_any(const entt::entity ent)
	{
		return internal::get_registry().get().any_of<Args...>(ent);
	}

	template<typename... Args>
	bool contains_any(const entity_view& ent)
	{
		return contains_any<Args...>(ent.get());
	}

	void init();
	void destroy();
	void tick(float dt);

	void clear();
	void free();

    const std::chrono::nanoseconds& get_last_serialize_time();

	void deserialize_state(stl::string_view state_name);
	void serialize_state(stl::string_view state_name);

	void string_serialize(stl::tree_string_map& data);
	void string_deserialize(const stl::tree_string_map& data);

	void serialize(stl::stream_vector& data);
	void deserialize(stl::stream_vector& data);

	bool is_valid(entity_view ent);
	bool is_null(entity_view ent);
	
	entity_view create();
	void mark_as_garbage(entity_view ent);

	entity_view get_entity_from_body(const b2Body* body);
	ark_float_vec2 get_position(entity_view entity);

	entity_view add_texture(
		entity_view ent,
		stl::string_view path
	);
	
	entity_view add_phys_body(
		entity_view ent,
		ark_float_vec2 vel,
		ark_float_vec2 pos,
		ark_float_vec2 size,
		physics::body_type type = physics::body_type::dynamic_body,
		material::shape shape = material::shape::box,
		material::type mat = material::type::solid
	);
}
