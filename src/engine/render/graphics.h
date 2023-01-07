#pragma once

namespace Asura::Graphics
{
	void Init();

	void Destroy();
	void Tick(float dt);
	void draw(float dt);
	
	void DrawPhysObject(b2Body* object, const ImColor& clr);
	void DrawPhysObjectCircle(b2Body* object, const ImColor& clr);
	void DrawConvexFilled(ImDrawList* draw_list, const Math::FVec2* points, const int points_count, ImU32 col);

	void DrawRect(ImColor color, const Math::FRect& Rect, bool filled = true);
	void DrawTextureRect(ResourcesManager::id_t resource_id, const Math::FRect& Rect);
	void DrawBackground(ResourcesManager::id_t resource_id, bool UseParallax);
	void DrawTextureObject(Physics::PhysicsBody* Object, ResourcesManager::id_t resource_id);

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
