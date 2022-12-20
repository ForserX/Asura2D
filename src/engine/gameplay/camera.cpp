#include "pch.h"

using namespace Asura;
using namespace Asura::GamePlay;

// Base camera data
static Math::FVec2 cam_center = {};
float cam_zoom = 0.f;
float cam_rotation = 0.f;
static float scaled_cam_zoom = {};
static int64_t cam_width = {};
static int64_t cam_height = {};

static EntityView attached_entity = {};
static bool attached = false;
constexpr bool test_world_transform = false;

// For parallax background
static Math::IRect OffsetMove = { 0, 0, 0, 0};

// For Input system
static int64_t CameraInputKeyID = 0;
static int64_t CameraInputWheelID = 0;

auto camera_mouse_key_change = [](int16_t scan_code, Asura::Input::key_state state)
{
	switch (scan_code)
	{
	case SDL_SCANCODE_MOUSE_LEFT:
	{
		if (Asura::Input::IsKeyPressed(SDL_SCANCODE_LCTRL))
		{
			if (state == Asura::Input::key_state::hold)
			{
				const auto& mouse_delta = Asura::Input::GetMouseDelta();
				Camera::Move(GamePlay::MoveWays::left, (mouse_delta.x * 0.05f));
				Camera::Move(GamePlay::MoveWays::up, (mouse_delta.y * 0.05f));
			}
		}
		break;
	}

	case SDL_SCANCODE_LEFT:  Camera::Move(GamePlay::MoveWays::left, 1.f);  break;
	case SDL_SCANCODE_RIGHT: Camera::Move(GamePlay::MoveWays::right, 1.f); break;
	case SDL_SCANCODE_UP:	 Camera::Move(GamePlay::MoveWays::up, 1.f);	   break;
	case SDL_SCANCODE_DOWN:  Camera::Move(GamePlay::MoveWays::down, 1.f);  break;
	default:															   break;
	}
};

auto camera_mouse_wheel_change = [](int16_t scan_code, float state)
{
	switch (scan_code)
	{
	case SDL_SCANCODE_MOUSEWHEEL: Camera::Zoom((-1.f * state) * 2.f); break;
	default:					  break;
	}
};

void Camera::Init()
{
	CameraInputKeyID = Input::Emplace(camera_mouse_key_change);
	CameraInputWheelID = Input::Emplace(camera_mouse_wheel_change);

	ResetHW();
	ResetView();
}

void Camera::Destroy()
{
	Asura::Input::Erase(CameraInputWheelID);
	Asura::Input::Erase(CameraInputKeyID);
}

void Camera::Tick(float dt)
{
	const float delta_size =  static_cast<float>(cam_height) / static_cast<float>(cam_width);
	scaled_cam_zoom = cam_zoom * delta_size;
	
	if (attached) 
	{
		if (!Entities::IsValid(attached_entity)) 
		{
			Detach();
		} 
		else
		{
			cam_center = Entities::GetPosition(attached_entity);
		}
	}
    
    if constexpr (test_world_transform) 
	{
        auto mouse_vec = Math::FVec2(Input::GetMousePos().x, Input::GetMousePos().y);
        mouse_vec = World2Screen(Screen2World(mouse_vec));
        Math::FRect Rect = { Math::FVec2(mouse_vec.x - 10.f, mouse_vec.y - 10.f), Math::FVec2(mouse_vec.x + 10.f, mouse_vec.y + 10.f) };
        Graphics::DrawRect(ImColor(1.f, 1.f, 1.f, 1.f), Rect);
    }
}

void Camera::Move(MoveWays move, float point)
{
	Detach();

	switch (move) 
	{
	case MoveWays::left:  cam_center[0] -= scaled_cam_zoom * point; OffsetMove.values[0] += 5; OffsetMove.values[2] += 5; break;
	case MoveWays::right: cam_center[0] += scaled_cam_zoom * point; OffsetMove.values[0] -= 5; OffsetMove.values[2] -= 5; break;
	case MoveWays::up:    cam_center[1] += scaled_cam_zoom * point; OffsetMove.values[1] += 5; OffsetMove.values[3] += 5; break;
	case MoveWays::down:  cam_center[1] -= scaled_cam_zoom * point; OffsetMove.values[1] -= 5; OffsetMove.values[3] -= 5; break;
	}

	auto ValidOffset = [](int16_t& Data)
	{
		if (Data > 50)
			Data = 50;

		if (Data < -50)
			Data = -50;
	};

	ValidOffset(OffsetMove.values[0]);
	ValidOffset(OffsetMove.values[1]);
	ValidOffset(OffsetMove.values[2]);
	ValidOffset(OffsetMove.values[3]);

	BackgroundParallax = { -50, -50, window_width + 50, window_height + 50 };
	BackgroundParallax += OffsetMove;
}

void Camera::Zoom(float value)
{
	cam_zoom += value * static_cast<float>(cam_height) / (static_cast<float>(cam_width));
	cam_zoom = std::clamp(cam_zoom, 15.f, 40.f);
}

bool Camera::IsAttached()
{
	return attached;
}

void Camera::Attach(EntityView entity)
{
	if (attached)
	{
		Detach();
	}

	attached_entity = entity;
	attached = true;
}

void Camera::Detach()
{
	attached = false;
	attached_entity = {};
}

void Camera::ResetView()
{
	cam_zoom = 30.f;
	scaled_cam_zoom = 16;

    cam_center = {496, 320};
}

void Camera::ResetHW()
{
	cam_width = window_width;
	cam_height = window_height;
}

const Math::FVec2& Camera::Position()
{
	return cam_center;
}

Math::FVec2 Camera::Screen2World(const Math::FVec2& screenPoint)
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

Math::FVec2 Camera::World2Screen(const Math::FVec2& worldPoint)
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

float Camera::ScaleFactor(float in)
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

bool Asura::GamePlay::Camera::CanSee(Math::FVec2 Pos)
{
	float dx = (float)cam_width;
	float dy = (float)cam_height;

	float x = cam_center.x - dx;
	float y = cam_center.y - dy;
	float h = cam_center.x + dx;
	float w = cam_center.y + dy;

	bool SeeX = (ScaleFactor(Pos.x) < w) && (ScaleFactor(Pos.x) > x);
	bool SeeY = (ScaleFactor(Pos.y) < h) && (ScaleFactor(Pos.y) > y);

	return SeeX && SeeY;
}
