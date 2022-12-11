#include "pch.h"

using namespace Asura;

static Math::FVec2 cam_center = {};
float cam_zoom = 0.f;
float cam_rotation = 0.f;
static float scaled_cam_zoom = {};
static int64_t cam_width = {};
static int64_t cam_height = {};

static entity_view attached_entity = {};
static bool attached = false;
constexpr bool test_world_transform = false;

void Camera::Init()
{
	reset_wh();
	reset_view();
}

void Camera::Destroy()
{

}

void Camera::Tick(float dt)
{
	const float delta_size =  static_cast<float>(cam_height) / static_cast<float>(cam_width);
	scaled_cam_zoom = cam_zoom * delta_size;
	
	if (attached) 
	{
		if (!entities::IsValid(attached_entity)) 
		{
			detach();
		} else {
			cam_center = entities::get_position(attached_entity);
		}
	}
    
    if constexpr (test_world_transform) {
        auto mouse_vec = Math::FVec2(input::get_mouse_pos().x, input::get_mouse_pos().y);
        mouse_vec = world_to_screen(screen_to_world(mouse_vec));
        Math::FRect Rect = { Math::FVec2(mouse_vec.x - 10.f, mouse_vec.y - 10.f), Math::FVec2(mouse_vec.x + 10.f, mouse_vec.y + 10.f) };
        graphics::draw_rect(ImColor(1.f, 1.f, 1.f, 1.f), Rect);
    }
}

void Camera::move(cam_move move, float point)
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

void Camera::zoom(float value)
{
	cam_zoom += value * static_cast<float>(cam_height) / (static_cast<float>(cam_width));
	cam_zoom = std::clamp(cam_zoom, 1.f, 100.f);
}

bool Camera::is_attached()
{
	return attached;
}

void Camera::attach(entity_view entity)
{
	if (attached) {
		detach();
	}

	attached_entity = entity;
	attached = true;
}

void Camera::detach()
{
	attached = false;
	attached_entity = {};
}

void Camera::reset_view()
{
	cam_zoom = 30.f;
	scaled_cam_zoom = 16;

    cam_center = {496, 320};
}

void Camera::reset_wh()
{
	cam_width = window_width;
	cam_height = window_height;
}

const Math::FVec2& Camera::camera_position()
{
	return cam_center;
}

Math::FVec2 Camera::screen_to_world(const Math::FVec2& screenPoint)
{
    const float w = static_cast<float>(cam_width);
    const float h = static_cast<float>(cam_height);
    const float u = screenPoint.x / w;
    const float v = (h - screenPoint.y) / h;
    const float ratio = w / h;
    Math::FVec2 extents(ratio * 25.0f, 25.0f);
    extents *= scaled_cam_zoom;

    const Math::FVec2 lower = cam_center - extents;
    const Math::FVec2 upper = cam_center + extents;

    Math::FVec2 pw;
    pw[0] = (1.0f - u) * lower.x + u * upper.x;
    pw[1] = (1.0f - v) * lower.y + v * upper.y;
    return pw;
}

Math::FVec2 Camera::world_to_screen(const Math::FVec2& worldPoint)
{
    const float w = static_cast<float>(cam_width);
    const float h = static_cast<float>(cam_height);
    const float ratio = w / h;
    Math::FVec2 extents(ratio * 25.0f, 25.0f);
    extents *= scaled_cam_zoom;

    const Math::FVec2 lower = cam_center - extents;
    const Math::FVec2 upper = cam_center + extents;
    const float u = (worldPoint.x - lower.x) / (upper.x - lower.x);
    const float v = (worldPoint.y - lower.y) / (upper.y - lower.y);

    Math::FVec2 ps;
    ps[0] = u * w;
    ps[1] = (1.0f - v) * h;
    return ps;
}

float Camera::scale_factor(float in)
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