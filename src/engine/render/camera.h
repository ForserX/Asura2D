#pragma once

namespace Asura::Camera
{
	enum class cam_move : int8_t 
	{
		left,
		right,
		up,
		down
	};

	void Init();
	void Destroy();
	void Tick(float dt);

	void move(cam_move move, float point);
	void zoom(float value);

	bool is_attached();
	void attach(EntityView entity);
	void detach();
	
	void reset_view();
	void reset_wh();

	const Math::FVec2& camera_position();
    Math::FVec2 screen_to_world(const Math::FVec2& screenPoint);
    Math::FVec2 world_to_screen(const Math::FVec2& worldPoint);
	float scale_factor(float in);

}
