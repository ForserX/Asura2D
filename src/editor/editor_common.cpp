#include "pch.h"
#include "editor_common.h"

using namespace asura;

static entity_view current_entt_object;

static input::on_key_change editor_key_click_event;

static math::fvec2 start_mouse_position_absolute = {};
static math::fvec2 last_mouse_position_absolute = {};

bool ark_editor_mode = false;

void editor::init()
{
	auto editor_key = [](int16_t scan_code, asura::input::key_state state)
	{
		if (!ark_editor_mode)
			return;

		if (scan_code == SDL_SCANCODE_MOUSE_LEFT)
		{
			if (state == asura::input::key_state::press) {
				object::create_fake();
			}
			else if (state == asura::input::key_state::hold) {
				object::update_fake();
			}
			else {
				object::make_phys();
			}
		}
	};

	editor_key_click_event = input::subscribe_key_event(editor_key);
}

void editor::destroy()
{
	asura::input::unsubscribe_key_event(editor_key_click_event);
}

//////////////////////////////////////////////////////////////////////////////////////
// Object working code

void editor::object::create_fake()
{
	start_mouse_position_absolute = ImGui::GetMousePos();
	current_entt_object = entities::add_scene_component(entities::create());
}

void editor::object::update_fake()
{
	last_mouse_position_absolute = ImGui::GetMousePos();
	auto ent = entities::try_get<entities::scene_component>(current_entt_object);
	ent->size = start_mouse_position_absolute - last_mouse_position_absolute;

	math::fvec2 try_pos = start_mouse_position_absolute;
	try_pos -= ent->size / 2;

	ent->transform.set_position(try_pos);
}

void editor::object::make_phys()
{
	auto ent = entities::try_get<entities::scene_component>(current_entt_object);

	math::fvec2 start_world = camera::screen_to_world(start_mouse_position_absolute);
	math::fvec2 last_world = camera::screen_to_world(last_mouse_position_absolute);

	ent->size.x = std::max(start_world.x, last_world.x) - std::min(start_world.x, last_world.x);
	ent->size.y = std::max(start_world.y, last_world.y) - std::min(start_world.y, last_world.y);

	math::fvec2 half_size = ent->size / 2;
	math::fvec2 world_pos = {};

	world_pos.x = std::min(start_mouse_position_absolute.x, last_mouse_position_absolute.x);
	world_pos.y = std::max(start_mouse_position_absolute.y, last_mouse_position_absolute.y);
	world_pos = camera::screen_to_world(world_pos);

	entities::add_phys_body(current_entt_object, {}, world_pos + half_size, half_size);
}