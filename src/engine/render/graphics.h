#pragma once

namespace asura::graphics
{
	void init();

	void destroy();
	void tick(float dt);
	void draw(float dt);
	
	void draw_physical_object(b2Body* object, const ImColor& clr);
	void draw_physical_circle_object(b2Body* object, const ImColor& clr);
	void draw_convex_poly_filled(ImDrawList* draw_list, const math::fvec2* points, const int points_count, ImU32 col);

	void draw_rect(ImColor color, const math::frect& rect, bool filled = true);
	void draw_textured_rect(resources::id_t resource_id, const math::frect& rect);
	void draw_background(resources::id_t resource_id);

	namespace theme
	{
		enum class style : int64_t
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
