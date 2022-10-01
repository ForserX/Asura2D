#pragma once

namespace ark::entities
{
	struct dont_free_after_reset {};	// created for engine/base game entities, which requirement is to be stable 
	struct background_flag {};
	struct drawable_flag {};
	struct ground_flag {};

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
