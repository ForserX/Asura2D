#include "pch.h"

using namespace ark;

static ark_float_vec2 cam_center;
float cam_zoom;
static float scaled_cam_zoom;
static int64_t cam_width;
static int64_t cam_height;

static entity_view attached_entity;
static bool attached = false;

auto camera_mouse_key_change = [](int16_t scan_code, input::key_state state)
{
	switch (scan_code) {
		case SDL_SCANCODE_MOUSE_RIGHT: {
			if (state == input::key_state::press) {
				const auto screen_cords = ark_float_vec2(input::get_mouse_pos().x, input::get_mouse_pos().y);
				const physics::physics_body* body = physics::hit_test(camera::screen_to_world(screen_cords));
				if (body != nullptr) {
					camera::attach(entities::get_entity_from_body(body->get_body()));
				} else {
					camera::detach();
				}
			}
			break;
		}
		case SDL_SCANCODE_LEFT:
			move(camera::cam_move::left, 1.f);
			break;
		case SDL_SCANCODE_RIGHT:
			move(camera::cam_move::right, 1.f);
			break;
		case SDL_SCANCODE_UP:
			move(camera::cam_move::up, 1.f);
			break;
		case SDL_SCANCODE_DOWN:
			move(camera::cam_move::down, 1.f);
			break;
		default:
			break;
	}
};

void camera::init()
{
	reset_wh();
	reset_view();

	input::subscribe_key_event(camera_mouse_key_change);
}

void camera::destroy()
{
	input::unsubscribe_key_event(camera_mouse_key_change);
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

	cam_center.Set(496, 320);
}

void
camera::reset_wh()
{
	cam_width = ui::get_cmd_int("window_width");
	cam_height = ui::get_cmd_int("window_height");
}

ark_float_vec2
camera::camera_position()
{
	return cam_center;
}

ark_float_vec2
camera::screen_to_world(const ark_float_vec2& screenPoint)
{
	const float w = static_cast<float>(cam_width);
	const float h = static_cast<float>(cam_height);
	const float u = screenPoint.x / w;
	const float v = (h - screenPoint.y) / h;
	const float ratio = w / h;
	ark_float_vec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

	const ark_float_vec2 lower = cam_center - extents;
	const ark_float_vec2 upper = cam_center + extents;

	ark_float_vec2 pw;
	pw.x = (1.0f - u) * lower.x + u * upper.x;
	pw.y = (1.0f - v) * lower.y + v * upper.y;
	return pw;
}

ark_float_vec2
camera::world_to_screen(const ark_float_vec2& worldPoint)
{
	const float w = static_cast<float>(cam_width);
	const float h = static_cast<float>(cam_height);
	const float ratio = w / h;
	ark_float_vec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

	const ark_float_vec2 lower = cam_center - extents;
	const ark_float_vec2 upper = cam_center + extents;
	const float u = (worldPoint.x - lower.x) / (upper.x - lower.x);
	const float v = (worldPoint.y - lower.y) / (upper.y - lower.y);

	ark_float_vec2 ps;
	ps.x = u * w;
	ps.y = (1.0f - v) * h;
	return ps;
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

void
camera::build_projection_matrix(float* m, float zBias)
{
	const float w = static_cast<float>(cam_width);
	const float h = static_cast<float>(cam_height);
	const float ratio = w / h;
	ark_float_vec2 extents(ratio * 25.0f, 25.0f);
	extents *= scaled_cam_zoom;

	const ark_float_vec2 lower = cam_center - extents;
	const ark_float_vec2 upper = cam_center + extents;

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
