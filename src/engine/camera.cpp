#include "pch.h"
#include "camera.h"

b2Vec2 cam_center;
float cam_zoom;
int64_t cam_width;
int64_t cam_height;

void ark::camera::init()
{
	reset_wh();
	reset_view();
}

void ark::camera::destroy()
{
}

void ark::camera::tick(float dt)
{
}

void ark::camera::move(cam_move move, float point)
{
	switch (move)
	{
	case cam_move::left:  cam_center.x += cam_zoom * point; break;
	case cam_move::right: cam_center.x -= cam_zoom * point; break;
	case cam_move::up:    cam_center.y += cam_zoom * point; break;
	case cam_move::down:  cam_center.y -= cam_zoom * point; break;

	}
}

void ark::camera::reset_view()
{
	cam_center.Set(cam_width / 2, cam_height / 2);
	cam_zoom = 16.f;
}

void ark::camera::reset_wh()
{
	cam_width = ui::get_cmd_int("window_width");
	cam_height = ui::get_cmd_int("window_height");
}

ark_float_vec2 ark::camera::screen2world(const b2Vec2& screenPoint)
{
	float w = float(cam_width);
	float h = float(cam_height);
	float u = screenPoint.x / w;
	float v = (h - screenPoint.y) / h;

	float ratio = w / h;
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= cam_zoom;

	b2Vec2 lower = cam_center - extents;
	b2Vec2 upper = cam_center + extents;

	b2Vec2 pw;
	pw.x = (1.0f - u) * lower.x + u * upper.x;
	pw.y = (1.0f - v) * lower.y + v * upper.y;
	return pw;
}

ark_float_vec2 ark::camera::world2screen(const b2Vec2& worldPoint)
{
	float w = float(cam_width);
	float h = float(cam_height);
	float ratio = w / h;
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= cam_zoom;

	b2Vec2 lower = cam_center - extents;
	b2Vec2 upper = cam_center + extents;

	float u = (worldPoint.x - lower.x) / (upper.x - lower.x);
	float v = (worldPoint.y - lower.y) / (upper.y - lower.y);

	b2Vec2 ps;
	ps.x = u * w;
	ps.y = (1.0f - v) * h;
	return ps;
}

void ark::camera::build_projection_matrix(float* m, float zBias)
{
	float w = float(cam_width);
	float h = float(cam_height);
	float ratio = w / h;
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= cam_zoom;

	b2Vec2 lower = cam_center - extents;
	b2Vec2 upper = cam_center + extents;

	m[0] = 2.0f / (upper.x - lower.x);
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 2.0f / (upper.y - lower.y);
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = 1.0f;
	m[11] = 0.0f;

	m[12] = -(upper.x + lower.x) / (upper.x - lower.x);
	m[13] = -(upper.y + lower.y) / (upper.y - lower.y);
	m[14] = zBias;
	m[15] = 1.0f;
}
