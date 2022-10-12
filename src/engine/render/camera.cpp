#include "pch.h"

using namespace ark;

static math::fvec2 cam_center = {};
float cam_zoom = 0.f;
float cam_rotation = 0.f;
static float scaled_cam_zoom = {};
static int64_t cam_width = {};
static int64_t cam_height = {};

static entity_view attached_entity = {};
static bool attached = false;

void camera::init()
{
	reset_wh();
	reset_view();
}

void camera::destroy()
{

}

void camera::tick(float dt)
{
	const float delta_size =  static_cast<float>(cam_height) / static_cast<float>(cam_width);
	scaled_cam_zoom = cam_zoom * delta_size;
	
	if (attached) {
		if (!entities::is_valid(attached_entity)) {
			detach();
		} else {
			cam_center = entities::get_position(attached_entity);
		}
	}
}

void camera::move(cam_move move, float point)
{
	detach();
	switch (move) {
	case cam_move::left:
		cam_center.x -= scaled_cam_zoom * point;
		break;
	case cam_move::right:
		cam_center.x += scaled_cam_zoom * point;
		break;
	case cam_move::up:
		cam_center.y += scaled_cam_zoom * point;
		break;
	case cam_move::down:
		cam_center.y -= scaled_cam_zoom * point;
		break;
	}
}

void 
camera::zoom(float value)
{
	cam_zoom += value * static_cast<float>(cam_height) / (static_cast<float>(cam_width));
	cam_zoom = std::clamp(cam_zoom, 1.f, 100.f);
}

bool
camera::is_attached()
{
	return attached;
}

void
camera::attach(entity_view entity)
{
	if (attached) {
		detach();
	}

	attached_entity = entity;
	attached = true;
}

void
camera::detach()
{
	attached = false;
	attached_entity = {};
}

void
camera::reset_view()
{
	cam_zoom = 30.f;
	scaled_cam_zoom = 16;

    cam_center = {0, 0};
}

void
camera::reset_wh()
{
	cam_width = ui::get_cmd_int("window_width");
	cam_height = ui::get_cmd_int("window_height");
}

const math::fvec2&
camera::camera_position()
{
	return cam_center;
}

math::fvec2
camera::screen_to_world(const math::fvec2& screenPoint)
{
	const float w = static_cast<float>(cam_width);
	const float h = static_cast<float>(cam_height);
	const float u = screenPoint.x / w;
	const float v = (h - screenPoint.y) / h;
	const float ratio = w / h;
	math::fvec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

	const auto lower = cam_center - extents;
	const auto upper = cam_center + extents;
    
	return math::fvec2((1.0f - u) * lower.x + u * upper.x, (1.0f - v) * lower.y + v * upper.y);
}

math::fvec2
camera::world_to_screen(const math::fvec2& worldPoint)
{
	const float w = static_cast<float>(cam_width);
	const float h = static_cast<float>(cam_height);
	const float ratio = w / h;
	math::fvec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

	const auto lower = cam_center - extents;
	const auto upper = cam_center + extents;
	const float u = (worldPoint.x - lower.x) / (upper.x - lower.x);
	const float v = (worldPoint.y - lower.y) / (upper.y - lower.y);
    
    return math::fvec2(u * w, (1.0f - v) * h);
}

float
camera::scale_factor(float in)
{
	const float w = static_cast<float>(cam_width);
	const float h = static_cast<float>(cam_height);
	const float ratio = w / h;
	const float extents = ratio * 25.0f;
	const float lower = w - extents;
	const float u = (in - lower) / (w + extents - lower);

	float ws = std::abs(u);
	ws *= 1.f / scaled_cam_zoom;
	ws *= 1.066f; // magic number

	return in * ws;
}

/*
void
camera::build_projection_matrix(float* m, float zBias)
{
	const float w = static_cast<float>(cam_width);
	const float h = static_cast<float>(cam_height);
	const float ratio = w / h;
	math::fvec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

	const auto lower = cam_center - extents;
	const auto upper = cam_center + extents;

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
*/
