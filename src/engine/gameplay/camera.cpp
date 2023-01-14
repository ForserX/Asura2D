#include "pch.h"

using namespace Asura;
using namespace Asura::GamePlay;

// Base camera data
float CamZoom = 0.f;

static Math::FVec2 CamCenter = {};
static float CamScaledZoom = 0.f;
static int64_t cam_width = 0;
static int64_t cam_height = 0;

static EntityView AttachedEntity = {};
static bool Attached = false;

// For Input system
static int64_t CameraInputKeyID = 0;
static int64_t CameraInputWheelID = 0;

auto CameraMouseKeyChange = [](int16_t scan_code, Input::State state)
{
	auto action = Asura::Input::Get(scan_code, state);
	
	switch (action)
	{
	case Input::eActions::CameraMoveLeft:  Camera::Move(GamePlay::MoveWays::left,	0.21f);  break;
	case Input::eActions::CameraMoveRight: Camera::Move(GamePlay::MoveWays::right,	0.21f); break;
	case Input::eActions::CameraMoveUp:	   Camera::Move(GamePlay::MoveWays::up,		0.21f);	  break;
	case Input::eActions::CameraMoveDown:  Camera::Move(GamePlay::MoveWays::down,	0.21f);  break;
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
	
	if (Attached) 
	{
		if (!Entities::IsValid(AttachedEntity))
		{
			Detach();
		} 
		else
		{
			CamCenter = Entities::GetPosition(AttachedEntity);
		}
	}
}

void Camera::Move(MoveWays move, float point)
{
	Detach();

	switch (move) 
	{
	case MoveWays::left:  CamCenter[0] -= CamScaledZoom * point; break;
	case MoveWays::right: CamCenter[0] += CamScaledZoom * point; break;
	case MoveWays::up:    CamCenter[1] += CamScaledZoom * point; break;
	case MoveWays::down:  CamCenter[1] -= CamScaledZoom * point; break;
	}
}

void Camera::Zoom(float value)
{
	CamZoom += value * static_cast<float>(cam_height) / (static_cast<float>(cam_width));
	CamZoom = std::clamp(CamZoom, 15.f, 40.f);
}

bool Camera::IsAttached()
{
	return Attached;
}

void Camera::Attach(EntityView entity)
{
	if (Attached)
	{
		Detach();
	}

	AttachedEntity = entity;
	Attached = true;
}

void Camera::Detach()
{
	Attached = false;
	AttachedEntity = {};
}

void Camera::ResetView()
{
	CamZoom = 26.f;
	CamScaledZoom = 16;

    CamCenter = {650, 440};
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
    ws *= 1.03f; // magic number

    return in * ws;
}

float Camera::Distance(float x, float x2)
{
	return std::abs(GamePlay::Camera::ScaleFactor(x2) - GamePlay::Camera::ScaleFactor(x));
}

bool Camera::CanSee(Math::FVec2 Pos)
{
	auto ScreenPos = World2Screen(Pos);

	bool SeeX = ScreenPos.x < fwindow_width && ScreenPos.x > 0;
	bool SeeY = ScreenPos.y < fwindow_height && ScreenPos.y > 0;

	return SeeX && SeeY;
}
