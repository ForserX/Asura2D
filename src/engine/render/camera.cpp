#include "pch.h"

using namespace asura;

static math::fvec2 cam_center = {};
float cam_zoom = 0.f;
float cam_rotation = 0.f;
static float scaled_cam_zoom = {};
static int64_t cam_width = {};
static int64_t cam_height = {};

static entity_view attached_entity = {};
static bool attached = false;
constexpr bool test_world_transform = false;

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
	
	if (attached) 
	{
		if (!entities::is_valid(attached_entity)) 
		{
			detach();
		} else {
			cam_center = entities::get_position(attached_entity);
		}
	}
    
    if constexpr (test_world_transform) {
        auto mouse_vec = math::fvec2(input::get_mouse_pos().x, input::get_mouse_pos().y);
        mouse_vec = world_to_screen(screen_to_world(mouse_vec));
        math::frect rect = { math::fvec2(mouse_vec.x - 10.f, mouse_vec.y - 10.f), math::fvec2(mouse_vec.x + 10.f, mouse_vec.y + 10.f) };
        graphics::draw_rect(ImColor(1.f, 1.f, 1.f, 1.f), rect);
    }
}

void camera::move(cam_move move, float point)
{
	detach();
	switch (move) 
	{
	case cam_move::left:
		cam_center[0] -= scaled_cam_zoom * point;
		break;
	case cam_move::right:
		cam_center[0] += scaled_cam_zoom * point;
		break;
	case cam_move::up:
		cam_center[1] += scaled_cam_zoom * point;
		break;
	case cam_move::down:
		cam_center[1] -= scaled_cam_zoom * point;
		break;
	}
}

void camera::zoom(float value)
{
	cam_zoom += value * static_cast<float>(cam_height) / (static_cast<float>(cam_width));
	cam_zoom = std::clamp(cam_zoom, 1.f, 100.f);
}

bool camera::is_attached()
{
	return attached;
}

void camera::attach(entity_view entity)
{
	if (attached) {
		detach();
	}

	attached_entity = entity;
	attached = true;
}

void camera::detach()
{
	attached = false;
	attached_entity = {};
}

void camera::reset_view()
{
	cam_zoom = 30.f;
	scaled_cam_zoom = 16;

    cam_center = {496, 320};
}

void camera::reset_wh()
{
	cam_width = ui::get_cmd_int("window_width");
	cam_height = ui::get_cmd_int("window_height");
}

const math::fvec2& camera::camera_position()
{
	return cam_center;
}

math::fvec2 camera::screen_to_world(const math::fvec2& screenPoint)
{
    const float w = static_cast<float>(cam_width);
    const float h = static_cast<float>(cam_height);
    const float u = screenPoint.x / w;
    const float v = (h - screenPoint.y) / h;
    const float ratio = w / h;
    math::fvec2 extents(ratio * 25.0f, 25.0f);
    extents *= scaled_cam_zoom;

    const math::fvec2 lower = cam_center - extents;
    const math::fvec2 upper = cam_center + extents;

    math::fvec2 pw;
    pw[0] = (1.0f - u) * lower.x + u * upper.x;
    pw[1] = (1.0f - v) * lower.y + v * upper.y;
    return pw;
}

math::fvec2 camera::world_to_screen(const math::fvec2& worldPoint)
{
    const float w = static_cast<float>(cam_width);
    const float h = static_cast<float>(cam_height);
    const float ratio = w / h;
    math::fvec2 extents(ratio * 25.0f, 25.0f);
    extents *= scaled_cam_zoom;

    const math::fvec2 lower = cam_center - extents;
    const math::fvec2 upper = cam_center + extents;
    const float u = (worldPoint.x - lower.x) / (upper.x - lower.x);
    const float v = (worldPoint.y - lower.y) / (upper.y - lower.y);

    math::fvec2 ps;
    ps[0] = u * w;
    ps[1] = (1.0f - v) * h;
    return ps;
}

float camera::scale_factor(float in)
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