#include "pch.h"

bool ark_editor_mode = false;

using namespace ark;

void
game::init()
{
	physics::init();
	systems::pre_init();
	systems::init();
	entities::init();
	level::init();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void
game::destroy()
{
	level::destroy();
	entities::destroy();
	systems::destroy();
	physics::destroy();
}

void
game::tick(float dt)
{
	OPTICK_EVENT("game tick")
	OPTICK_CATEGORY("systems tick", Optick::Category::GameLogic)
	
	{
		OPTICK_EVENT("systems pre tick")
		systems::pre_tick(dt);
	}

	{
		OPTICK_EVENT("physics tick")
		physics::tick(dt);
	}

	{
		OPTICK_EVENT("scene tick")
		scene::tick(dt);
	}

	{
		OPTICK_EVENT("systems tick")
		systems::tick(dt);
	}

	{
		OPTICK_EVENT("entities tick")
		entities::tick(dt);
	}

	{
		OPTICK_EVENT("systems post tick")
		systems::post_tick(dt);
	}
}

ark::input::on_key_change editor_key_click_event;
math::fvec2 start_mouse_position_absolute = {};

auto editor_key = [](int16_t scan_code, ark::input::key_state state) {
	if (scan_code == SDL_SCANCODE_MOUSE_LEFT)
	{
		if (state == ark::input::key_state::press) {
			start_mouse_position_absolute = start_mouse_position_absolute = camera::screen_to_world(ImGui::GetMousePos());
		}
		else if (state == ark::input::key_state::hold) {
			math::fvec2 last_mouse_position_absolute = ImGui::GetMousePos();
			graphics::draw_rect(0, math::frect(start_mouse_position_absolute.x, start_mouse_position_absolute.y, last_mouse_position_absolute.x, last_mouse_position_absolute.y));
		}
		else {

		}
	}
};

void
game::editor(bool state)
{
	if (state)
	{
		editor_key_click_event = input::subscribe_key_event(editor_key);
	}
	else
	{
		ark::input::unsubscribe_key_event(editor_key_click_event);
	}

	ark_editor_mode = state;
}