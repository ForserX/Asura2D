#pragma once

namespace Asura
{
	class Registry
	{
	private:
		entt::registry base_registry;

	public:
		entt::registry& Get() { return base_registry; }
		
		entt::entity Create() { return base_registry.create(); }
		void Destroy(const entt::entity& entity) { base_registry.destroy(entity); }
	};
	
	class EntityView
	{
	private:
		entt::entity ent;
		
	public:
		EntityView(entt::entity in_ent) : ent(in_ent) {}
		EntityView(EntityView const&) = default;
		EntityView() : ent(entt::null) {}
		~EntityView() = default;

		const entt::entity& Get() const { return ent; }
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

	bool IsValid(EntityView ent);
	bool IsNull(EntityView ent);
	
	EntityView Create();
	void MarkAsGarbage(const EntityView& ent);

	EntityView GetEntityByBbody(const b2Body* body);
    const Math::FVec2& get_position(const EntityView& ent);

	const EntityView& AddTexture(
		const EntityView& ent,
		stl::string_view path
	);
	
	const EntityView& AddPhysBody(
		const EntityView& ent,
		Math::FVec2 vel,
        Math::FVec2 pos,
        Math::FVec2 size,
		Physics::body_type type = Physics::body_type::ph_dynamic,
		Physics::Material::shape shape = Physics::Material::shape::box,
		Physics::Material::type mat = Physics::Material::type::solid
	);

	const EntityView& AddPhysBodyPreset(
		const EntityView& ent,
		Math::FVec2 pos,
		stl::string_view preset
	);

	const EntityView& AddSceneComponent(
		const EntityView& ent
	);
}
