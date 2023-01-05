#include "pch.h"

using namespace Asura;
using namespace Asura::GamePlay;

// Base camera data
float CamZoom = 0.f;

static Math::FVec2 CamCenter = {};
static float CamScaledZoom = 0.f;
static int64_t cam_width = 0;
static int64_t cam_height = 0;

static EntityView attached_entity = {};
static bool attached = false;
constexpr bool test_world_transform = false;

// For parallax background
static Math::IRect OffsetMove = { 0, 0, 0, 0};

// For Input system
static int64_t CameraInputKeyID = 0;
static int64_t CameraInputWheelID = 0;

auto CameraMouseKeyChange = [](int16_t scan_code, Input::State state)
{
	auto action = Asura::Input::Get(scan_code, state);
	
	switch (action)
	{

	/*
	case SDL_SCANCODE_MOUSE_LEFT:
	{
		if (Asura::Input::IsKeyPressed(SDL_SCANCODE_LCTRL))
		{
			if (state == Asura::Input::State::hold)
			{
				const auto& mouse_delta = Asura::Input::GetMouseDelta();
				Camera::Move(GamePlay::MoveWays::left, (mouse_delta.x * 0.05f));
				Camera::Move(GamePlay::MoveWays::up, (mouse_delta.y * 0.05f));
			}
		}
		break;
	}
	*/

	case Input::eActions::CameraMoveLeft:  Camera::Move(GamePlay::MoveWays::left, 1.f);  break;
	case Input::eActions::CameraMoveRight: Camera::Move(GamePlay::MoveWays::right, 1.f); break;
	case Input::eActions::CameraMoveUp:	   Camera::Move(GamePlay::MoveWays::up, 1.f);	  break;
	case Input::eActions::CameraMoveDown:  Camera::Move(GamePlay::MoveWays::down, 1.f);  break;
	default: break;
	}


};

auto CameraMouseWheelChange = [](int16_t scan_code, float state)
{
	switch (scan_code)
	{
	case GLFW_MOUSE_BUTTON_MIDDLE: Camera::Zoom((-1.f * state) * 2.f); break;
	default:					  break;
	}
};

void Camera::Init()
{

	for (int i = 0; i < 4; i++)
	{
		Input::Bind(GLFW_KEY_RIGHT + i, Input::State::Hold, (Input::eActions)((int)Input::eActions::CameraMoveRight + i));
	}

	CameraInputKeyID = Input::Emplace(CameraMouseKeyChange);
	CameraInputWheelID = Input::Emplace(CameraMouseWheelChange);

	ResetHW();
	ResetView();
}

void Camera::Destroy()
{
	Input::Erase(CameraInputWheelID);
	Input::Erase(CameraInputKeyID);
}

void Camera::Tick(float dt)
{
	const float delta_size =  static_cast<float>(cam_height) / static_cast<float>(cam_width);
	CamScaledZoom = CamZoom * delta_size;
	
	if (attached) 
	{
		if (!Entities::IsValid(attached_entity)) 
		{
			Detach();
		} 
		else
		{
			CamCenter = Entities::GetPosition(attached_entity);
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
	case MoveWays::left:  CamCenter[0] -= CamScaledZoom * point; OffsetMove.values[0] += 5; OffsetMove.values[2] += 5; break;
	case MoveWays::right: CamCenter[0] += CamScaledZoom * point; OffsetMove.values[0] -= 5; OffsetMove.values[2] -= 5; break;
	case MoveWays::up:    CamCenter[1] += CamScaledZoom * point; OffsetMove.values[1] += 5; OffsetMove.values[3] += 5; break;
	case MoveWays::down:  CamCenter[1] -= CamScaledZoom * point; OffsetMove.values[1] -= 5; OffsetMove.values[3] -= 5; break;
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
	CamZoom += value * static_cast<float>(cam_height) / (static_cast<float>(cam_width));
	CamZoom = std::clamp(CamZoom, 15.f, 40.f);
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
	CamZoom = 30.f;
	CamScaledZoom = 16;

    CamCenter = {496, 320};
}

void Camera::ResetHW()
{
	cam_width = window_width;
	cam_height = window_height;
}

const Math::FVec2& Camera::Position()
{
	return CamCenter;
}

Math::FVec2 Camera::Screen2World(const Math::FVec2& screenPoint)
{
    const float w = static_cast<float>(cam_width);
    const float h = static_cast<float>(cam_height);
    const float u = screenPoint.x / w;
    const float v = (h - screenPoint.y) / h;
    const float ratio = w / h;
    Math::FVec2 extents(ratio * 25.0f, 25.0f);
    extents *= CamScaledZoom;

    const Math::FVec2 lower = CamCenter - extents;
    const Math::FVec2 upper = CamCenter + extents;

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
    extents *= CamScaledZoom;

    const Math::FVec2 lower = CamCenter - extents;
    const Math::FVec2 upper = CamCenter + extents;
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
    ws *= 1.f / CamScaledZoom;
    ws *= 1.066f; // magic number

    return in * ws;
}

bool Asura::GamePlay::Camera::CanSee(Math::FVec2 Pos)
{
	auto ScreenPos = World2Screen(Pos);

	bool SeeX = ScreenPos.x < fwindow_width && ScreenPos.x > 0;
	bool SeeY = ScreenPos.y < fwindow_height && ScreenPos.y > 0;

	return SeeX && SeeY;
}
