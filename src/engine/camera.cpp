#include "pch.h"
#include "camera.h"

static ark_float_vec2 cam_center;
float cam_zoom;
static float scaled_cam_zoom;
static int64_t cam_width;
static int64_t cam_height;

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
	float delta_size =  (float)cam_height / (float)cam_width;
	scaled_cam_zoom = cam_zoom * delta_size;
}

void ark::camera::move(cam_move move, float point)
{
	switch (move)
	{
	case cam_move::left:  cam_center.x += scaled_cam_zoom * point; break;
	case cam_move::right: cam_center.x -= scaled_cam_zoom * point; break;
	case cam_move::up:    cam_center.y += scaled_cam_zoom * point; break;
	case cam_move::down:  cam_center.y -= scaled_cam_zoom * point; break;

	}
}

void ark::camera::reset_view()
{
	cam_zoom = 15.f;
	scaled_cam_zoom = 16;

	cam_center.Set(496, 320);
}

void ark::camera::reset_wh()
{
	cam_width = ui::get_cmd_int("window_width");
	cam_height = ui::get_cmd_int("window_height");
}

ark_float_vec2 ark::camera::camera_postion()
{
	return cam_center;
}

ark_float_vec2 ark::camera::screen2world(const b2Vec2& screenPoint)
{
	float w = float(cam_width);
	float h = float(cam_height);
	float u = screenPoint.x / w;
	float v = (h - screenPoint.y) / h;

	float ratio = w / h;
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

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
	extents *= scaled_cam_zoom;

	b2Vec2 lower = cam_center - extents;
	b2Vec2 upper = cam_center + extents;

	float u = (worldPoint.x - lower.x) / (upper.x - lower.x);
	float v = (worldPoint.y - lower.y) / (upper.y - lower.y);

	b2Vec2 ps;
	ps.x = u * w;
	ps.y = (1.0f - v) * h;
	return ps;
}

float ark::camera::scale_factor(float in)
{
	float w = float(cam_width);
	float h = float(cam_height);
	float ratio = w / h;

	float extents = ratio * 25.0f;
	float lower = w - extents;

	float u = (in - lower) / (w + extents - lower);

	float ws = std::abs(u);
	ws *= 1.f / scaled_cam_zoom;
	ws *= 1.066f; // magic number

	return in * ws;
}

void ark::camera::build_projection_matrix(float* m, float zBias)
{
	float w = float(cam_width);
	float h = float(cam_height);
	float ratio = w / h;
	b2Vec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

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
