#pragma once

namespace Asura::graphics
{
	void Init();

	void Destroy();
	void Tick(float dt);
	void draw(float dt);
	
	void draw_physical_object(b2Body* object, const ImColor& clr);
	void draw_physical_circle_object(b2Body* object, const ImColor& clr);
	void draw_convex_poly_filled(ImDrawList* draw_list, const Math::FVec2* points, const int points_count, ImU32 col);

	void draw_rect(ImColor color, const Math::FRect& Rect, bool filled = true);
	void draw_textured_rect(ResourcesManager::id_t resource_id, const Math::FRect& Rect);
	void draw_background(ResourcesManager::id_t resource_id);

	namespace theme
	{
		enum class style : int64_t
		{
			red,
			dark,
			white,
			invalid
		};

		void red();
		void dark();
		void change();
	}
}
