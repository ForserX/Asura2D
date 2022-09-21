#pragma once

namespace ark::entities
{
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
	
	struct physics_body_component
	{
		bool is_drawing = false;
		b2Body* body = nullptr;
	};

	struct screen_visual_component
	{
		std::vector<ark_int_vec2> points;
	};

	struct visual_component
	{
		ark_int_vec2 position;
		std::vector<ark_int_vec2> points;
	};
}
