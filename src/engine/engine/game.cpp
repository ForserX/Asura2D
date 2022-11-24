#include "pch.h"

bool ark_editor_mode = false;

using namespace ark;

ark::input::on_key_change editor_key_click_event;
math::fvec2 start_mouse_position_absolute = {};
math::fvec2 last_mouse_position_absolute = {};

ark::entity_view TestEditorObject;

void
game::init()
{
	physics::init();
	systems::pre_init();
	systems::init();
	entities::init();
	level::init();

	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	auto editor_key = [](int16_t scan_code, ark::input::key_state state) 
	{
		if (!ark_editor_mode)
			return;

		if (scan_code == SDL_SCANCODE_MOUSE_LEFT)
		{
			if (state == ark::input::key_state::press) {
				start_mouse_position_absolute = ImGui::GetMousePos();
				TestEditorObject = entities::add_scene_component(entities::create());
			}
			else if (state == ark::input::key_state::hold) {
				last_mouse_position_absolute = ImGui::GetMousePos();
				auto ent = entities::try_get<entities::scene_component>(TestEditorObject);
				ent->size = start_mouse_position_absolute - last_mouse_position_absolute;

				math::fvec2 try_pos = start_mouse_position_absolute;
				try_pos -= ent->size / 2;

				ent->transform.set_position(try_pos);
			}
			else {
				auto ent = entities::try_get<entities::scene_component>(TestEditorObject);

				math::fvec2 start_world = camera::screen_to_world(start_mouse_position_absolute);
				math::fvec2 last_world = camera::screen_to_world(last_mouse_position_absolute);

				ent->size.x = std::max(start_world.x, last_world.x) - std::min(start_world.x, last_world.x);
				ent->size.y = std::max(start_world.y, last_world.y) - std::min(start_world.y, last_world.y);

				math::fvec2 half_size = ent->size / 2;
				math::fvec2 world_pos = {};

				world_pos.x = std::min(start_mouse_position_absolute.x, last_mouse_position_absolute.x);
				world_pos.y = std::max(start_mouse_position_absolute.y, last_mouse_position_absolute.y);
				world_pos = camera::screen_to_world(world_pos);

				entities::add_phys_body(TestEditorObject, {}, world_pos + half_size, half_size);
			}
		}
	};

	editor_key_click_event = input::subscribe_key_event(editor_key);
}

void
game::destroy()
{
	ark::input::unsubscribe_key_event(editor_key_click_event);

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

void
game::editor(bool state)
{
	ark_editor_mode = state;
}