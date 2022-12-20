#pragma once

namespace Asura
{
	using EntityBase = entt::entity;

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
		EntityBase ent;
		
	public:
		EntityView(entt::entity in_ent) : ent(in_ent) {}
		EntityView(EntityView const&) = default;
		EntityView() : ent(entt::null) {}
		~EntityView() = default;

		const EntityBase& Get() const { return ent; }
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

	void Clear();
	void Free();

	bool IsValid(EntityView ent);
	bool IsNull(EntityView ent);
	
	EntityView Create();
	void MarkAsGarbage(const EntityView& ent);

	EntityBase GetEntityByBbody(const b2Body* body);
    const Math::FVec2& GetPosition(const EntityView& ent);

	void AddTexture(const EntityView& ent, stl::string_view path, bool Parallax = false);
	void AddPhysBody(const EntityView& ent, const Physics::body_parameters& BodyParams, bool IgnoreTest = false);
	void AddPhysBodyPreset(const EntityView& ent, Math::FVec2 pos, stl::string_view preset);
	void AddSceneComponent(const EntityView& ent);

	inline EntityView& CreateTexture(stl::string_view path, bool Parallax = false)
	{
		static EntityView Entt;

		Entt = Create();
		AddTexture(Entt, path, Parallax);
		return Entt;
	}

	inline EntityView& CreatePhysBody(const Physics::body_parameters& BodyParams, bool IgnoreTest = false)
	{
		static EntityView Entt;

		Entt = Create();
		AddPhysBody(Entt, BodyParams, IgnoreTest);
		return Entt;
	}

	inline EntityView& CreatePhysBodyPreset(Math::FVec2 pos, stl::string_view preset) 
	{
		static EntityView Entt;

		Entt = Create();
		AddPhysBodyPreset(Entt, pos, preset);
		return Entt;
	}

	inline EntityView& CreateSceneComponent()
	{
		static EntityView Entt;

		Entt = Create();
		AddSceneComponent(Entt);
		return Entt;
	}
}