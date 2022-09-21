#pragma once

namespace ark::graphics
{
	void init();

	void destroy();
	void tick(float dt);
	void draw(float dt);
	
	void draw_physical_object(b2Body* object, const ImColor& clr);
	void draw_convex_poly_filled(ImDrawList* draw_list, const ImVec2* points, const int points_count, ImU32 col);
	
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
