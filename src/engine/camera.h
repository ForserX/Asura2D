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

	ark_float_vec2 camera_position();
	ark_float_vec2 screen_to_world(const ark_float_vec2& screenPoint);
	ark_float_vec2 world_to_screen(const ark_float_vec2& worldPoint);
	float scale_factor(float in);

	void build_projection_matrix(float* m, float zBias);

}