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
        entity_view() : ent(entt::null) {}
		~entity_view() = default;

		const entt::entity& get() const { return ent; }
	};

	struct entity_desc
	{
        net::flag_type flags;
		uint8_t components_count;
		uint8_t reserved2;
	};
}

namespace ark::entities
{
	void init();
	void destroy();
	void tick(float dt);

	void clear();
	void free();

	bool is_valid(entity_view ent);
	bool is_null(entity_view ent);
	
	entity_view create();
	void mark_as_garbage(entity_view ent);

	entity_view get_entity_from_body(const b2Body* body);
    const math::fvec2& get_position(entity_view entity);

	entity_view add_texture(
		entity_view ent,
		stl::string_view path
	);
	
	entity_view add_phys_body(
		entity_view ent,
		math::fvec2 vel,
        math::fvec2 pos,
        math::fvec2 size,
		physics::body_type type = physics::body_type::dynamic_body,
		material::shape shape = material::shape::box,
		material::type mat = material::type::solid
	);
}

#define DECLARE_SERIALIZABLE_FLAGS \
    entities::background_flag, \
    entities::drawable_flag, \
    entities::ground_flag, \
    entities::level_flag, \
    entities::net_id_flag, \
    entities::net_controlled_flag

#define DECLARE_SERIALIZABLE_TYPES \
    DECLARE_SERIALIZABLE_FLAGS, \
    entities::draw_color_component, \
    entities::draw_gradient_component, \
    entities::draw_texture_component, \
    entities::scene_component, \
    entities::physics_body_component

#define DECLARE_SERIALIZABLE_ENTITY_TYPES \
    DECLARE_SERIALIZABLE_FLAGS, \
    DECLARE_SERIALIZABLE_TYPES

#define DECLARE_NON_SERIALIZABLE_TYPES \
    entities::garbage_flag, \
    entities::non_serializable_flag, \
    entities::dont_free_after_reset_flag

#define DECLARE_ENTITIES_TYPES \
    DECLARE_SERIALIZABLE_TYPES, \
    DECLARE_NON_SERIALIZABLE_TYPES

