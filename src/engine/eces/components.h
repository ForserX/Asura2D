#pragma once

namespace asura::entities
{
	// non-serializable, created for scheduled entities destroying
	struct garbage_flag
	{
		static constexpr net::flag_t flag = 1 << 0;
	};
	
	// non-serializable
	struct non_serializable_flag
	{
		static constexpr net::flag_t flag = 1 << 1;
	};
	
	// non-serializable, created for engine/base game entities, which requirement is to be stable 
	struct dont_free_after_reset_flag
	{
		static constexpr net::flag_t flag = 1 << 2;
	};
	
	// serializable, for background 
	struct background_flag
	{
		static constexpr net::flag_t flag = 1 << 3;
	};
	
	// serializable, for drawing
	struct drawable_flag
	{
		static constexpr net::flag_t flag = 1 << 4;
	};
	
	// serializable, for level specification
	struct level_flag
	{
		static constexpr net::flag_t flag = 1 << 5;
	};					

    constexpr uint32_t last_flag_index = 5;

	struct draw_color_component
	{
		ImColor color;
	};

	struct draw_gradient_component
	{
		ImColor first_color;
		ImColor second_color;
	};

	struct draw_texture_component
	{
        resources::id_t texture_resource;
	};

	struct scene_component
	{
        math::fvec2 size;
        math::transform transform;
	};
	
    struct camera_component
    {
        float cam_zoom;
        math::transform cam_transform;
    };

	struct physics_body_component
	{
		static constexpr bool custom_serialize = true;
		physics::physics_body* body = nullptr;
	};

	struct physics_joint_component
	{
		static constexpr bool custom_serialize = true;
		physics::physics_joint* joint = nullptr;
	};
}

// Don't forget to add this defines in game code
VISITABLE_STRUCT(asura::entities::draw_color_component, color);
VISITABLE_STRUCT(asura::entities::draw_gradient_component, first_color, second_color);
VISITABLE_STRUCT(asura::entities::draw_texture_component, texture_resource);
VISITABLE_STRUCT(asura::entities::scene_component, size, transform);
VISITABLE_STRUCT(asura::entities::camera_component, cam_zoom, cam_transform);