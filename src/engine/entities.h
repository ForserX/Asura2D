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
	
	class entity
	{
	private:
		entt::entity ent;
		
	public:
		entity(entt::entity in_ent) : ent(in_ent) {}
		entity(entity const&) = default;
		entity() = default;
		~entity() = default;

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
	void add_field(const entity& entt, Args &&...args)
	{
		add_field(entt.get(), std::forward<Args>(args)...);
	}

	template<typename Type>
	void erase_field(const entt::entity& entt)
	{
		get_registry().get().remove<Type>(entt);
	}
	
	template<typename Type>
	void erase_field(const entity& entt)
	{
		erase_field<Type>(entt.get());
	}

	entt::entity create_entity();
	void destroy_entity(const entt::entity& ent);
	
	entity create_phys_ground_entity(bool draw, b2Vec2 pos, b2Vec2 shape);
	entity create_phys_body_entity(bool draw, b2Vec2 pos, b2Vec2 shape);
	entity create_phys_body_entity_cricle(bool draw, b2Vec2 pos, b2Vec2 shape);
}
