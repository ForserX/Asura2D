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
}

namespace ark::entities
{
	registry& get_registry();
	
	template<typename Type, typename... Args>
	void add_field(const entt::entity& entt, Args &&...args)
	{
		get_registry().get().emplace<Type>(entt, std::forward<Args>(args)...);
	}
	
	template<typename Type, typename... Args>
	void add_field(const entity_view& entt, Args &&...args)
	{
		add_field(entt.get(), std::forward<Args>(args)...);
	}

	template<typename Type>
	void erase_field(const entt::entity& entt)
	{
		get_registry().get().remove<Type>(entt);
	}
	
	template<typename Type>
	void erase_field(const entity_view& entt)
	{
		erase_field<Type>(entt.get());
	}

	void init();
	void destroy();
	void tick(float dt);

	bool is_valid(entity_view ent);
	entity_view get_entity_from_body(const b2Body* body);
	
	entt::entity create_entity();
	void schedule_to_destroy_entity(const entt::entity& ent);

	ark_float_vec2 get_position(entity_view entity);
	
	entity_view create_phys_body(
		bool draw,
		ark_float_vec2 pos,
		ark_float_vec2 shape,
		physics::body_type type = physics::body_type::dynamic_body,
		material::type mat = material::type::solid
	);
}
