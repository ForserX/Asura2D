#pragma once

namespace ark::entities
{
	struct garbage_flag {};
	struct dont_free_after_reset {};	// non-serializable, created for engine/base game entities, which requirement is to be stable 
	struct background_flag {};			// serializable, for background 
	struct drawable_flag {};			// serializable, for drawing
	struct ground_flag {};				// serializable, for ground specification

	struct level_flag {};
	
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
		ImTextureID texture;
	};

	struct scene_component
	{
		ark_float_vec2 position;	
	};
	
	struct physics_body_component
	{
		physics::physics_body* body = nullptr;
	};
	
	struct visual_component
	{
		stl::vector<ark_float_vec2> points;
	};
}
