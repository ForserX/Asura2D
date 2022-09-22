#pragma once

namespace ark::camera
{
	enum class cam_move {
		left,
		right,
		up,
		down
	};

	void init();
	void destroy();
	void tick(float dt);

	void move(cam_move move, float point);

	void reset_view();
	void reset_wh();

	ark_float_vec2 camera_postion();
	ark_float_vec2 screen2world(const b2Vec2& screenPoint);
	ark_float_vec2 world2screen(const b2Vec2& worldPoint);
	float scale_factor(float in);

	void build_projection_matrix(float* m, float zBias);

}