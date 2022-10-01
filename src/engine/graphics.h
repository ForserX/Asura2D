#pragma once

namespace ark::graphics
{
	void init();

	void destroy();
	void tick(float dt);
	void draw(float dt);
	
	void draw_physical_object(b2Body* object, const ImColor& clr);
	void draw_physical_circle_object(b2Body* object, const ImColor& clr);
	void draw_convex_poly_filled(ImDrawList* draw_list, const ark_float_vec2* points, const int points_count, ImU32 col);

	void draw_rect(ImColor color, ark_float_vec2 p_min, ark_float_vec2 p_max, bool filled = true);
	void draw_background(ImTextureID texture_id);

	namespace theme
	{
		enum class style
		{
			red,
			dark,
			invalid
		};

		void red();
		void dark();
		void change();
	}
}
