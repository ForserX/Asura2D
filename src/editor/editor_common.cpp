#include "pch.h"
#include "editor_common.h"

using namespace Asura;
using namespace Asura::GamePlay;

static EntityView current_entt_object;

static int64_t EditorInputID = 0;

static Math::FVec2 start_mouse_position_absolute = {};
static Math::FVec2 last_mouse_position_absolute = {};

bool is_editor_mode = false;

void Editor::Init()
{
	auto editor_key = [](int16_t scan_code, Asura::Input::State state)
	{
		if (!is_editor_mode)
			return;

		if (scan_code == SDL_SCANCODE_MOUSE_LEFT)
		{
			if (state == Asura::Input::State::Press)
			{
				Object::CreateFake();
			}
			else if (state == Asura::Input::State::Hold)
			{
				Object::UpdateFake();
			}
			else
			{
				Object::MakeTry();
			}
		}
	};

	EditorInputID = Input::Emplace(editor_key);
}

void Editor::Destroy()
{
	Asura::Input::Erase(EditorInputID);
}

//////////////////////////////////////////////////////////////////////////////////////
// Object working code

void Editor::Object::CreateFake()
{
	start_mouse_position_absolute = ImGui::GetMousePos();
	current_entt_object = Entities::CreateSceneComponent();
}

void Editor::Object::UpdateFake()
{
	last_mouse_position_absolute = ImGui::GetMousePos();
	auto ent = Entities::TryGet<Entities::scene_component>(current_entt_object);
	ent->size = start_mouse_position_absolute - last_mouse_position_absolute;

	Math::FVec2 try_pos = start_mouse_position_absolute;
	try_pos -= ent->size / Math::FVec2{ 2.f, 2.f};

	ent->Transform.set_position(try_pos);
}

void Editor::Object::MakeTry()
{
	auto ent = Entities::TryGet<Entities::scene_component>(current_entt_object);

	Math::FVec2 start_world = Camera::Screen2World(start_mouse_position_absolute);
	Math::FVec2 last_world = Camera::Screen2World(last_mouse_position_absolute);

	ent->size.x = std::max(start_world.x, last_world.x) - std::min(start_world.x, last_world.x);
	ent->size.y = std::max(start_world.y, last_world.y) - std::min(start_world.y, last_world.y);

	Math::FVec2 half_size = ent->size / Math::FVec2{ 2.f, 2.f };
	Math::FVec2 world_pos = {};

	world_pos.x = std::min(start_mouse_position_absolute.x, last_mouse_position_absolute.x);
	world_pos.y = std::max(start_mouse_position_absolute.y, last_mouse_position_absolute.y);
	world_pos = Camera::Screen2World(world_pos);

	Physics::body_parameters CurrentBody
	(
		0, 0, {}, // Velocity
		world_pos + half_size, half_size // xy hw
	);

	Entities::AddPhysBody(current_entt_object, CurrentBody);
}