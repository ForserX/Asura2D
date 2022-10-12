#pragma once

namespace ark::camera
{
	enum class cam_move : int64_t {
		left,
		right,
		up,
		down
	};

	void init();
	void destroy();
	void tick(float dt);

	void move(cam_move move, float point);
	void zoom(float value);

	bool is_attached();
	void attach(entity_view entity);
	void detach();
	
	void reset_view();
	void reset_wh();

	const math::fvec2& camera_position();
    math::fvec2 screen_to_world(const math::fvec2& screenPoint);
    math::fvec2 world_to_screen(const math::fvec2& worldPoint);
	float scale_factor(float in);

}
