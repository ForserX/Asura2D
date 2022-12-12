#pragma once

namespace Asura
{
	class registry
	{
	private:
		entt::registry base_registry;

	public:
		entt::registry& get() { return base_registry; }
		
		entt::entity create() { return base_registry.create(); }
		void Destroy(const entt::entity& entity) { base_registry.destroy(entity); }
	};
	
	class entity_view
	{
	private:
		entt::entity ent;
		
	public:
		entity_view(entt::entity in_ent) : ent(in_ent) {}
		entity_view(entity_view const&) = default;
        entity_view() : ent(entt::null) {}
		~entity_view() = default;

		const entt::entity& get() const { return ent; }
	};

	struct entity_desc
	{
        net::flag_t flags;
		uint8_t components_count;
		uint8_t reserved2;
	};
}

namespace Asura::Entities
{
	void Init();
	void Destroy();
	void Tick(float dt);

	void clear();
	void free();

	bool IsValid(entity_view ent);
	bool IsNull(entity_view ent);
	
	entity_view Create();
	void MarkAsGarbage(const entity_view& ent);

	entity_view GetEntityByBbody(const b2Body* body);
    const Math::FVec2& get_position(const entity_view& ent);

	const entity_view& AddTexture(
		const entity_view& ent,
		stl::string_view path
	);
	
	const entity_view& AddPhysBody(
		const entity_view& ent,
		Math::FVec2 vel,
        Math::FVec2 pos,
        Math::FVec2 size,
		Physics::body_type type = Physics::body_type::ph_dynamic,
		material::shape shape = material::shape::box,
		material::type mat = material::type::solid
	);

	const entity_view& AddPhysBodyPreset(
		const entity_view& ent,
		Math::FVec2 pos,
		stl::string_view preset
	);

	const entity_view& AddSceneComponent(
		const entity_view& ent
	);
}
