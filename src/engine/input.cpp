#include "pch.h"

using namespace ark;

bool keys_states[4096] = {};

ark_int_vec2 current_mouse_pos = {};
ark_int_vec2 delta_mouse_pos = {};

std::unordered_map<int16_t, bool> changed_keys;
stl_shit::function_set<input::on_key_change> change_callbacks;

input::key_state
get_enum_from_state(int16_t scan_code)
{
	const bool new_state = changed_keys[scan_code];
	if (new_state != keys_states[scan_code]) {
		return new_state ? input::key_state::press : input::key_state::release;
	}

	return new_state ? input::key_state::hold : input::key_state::nothing;
}

void
input::init()
{
	
}

void
input::destroy()
{
}

void
input::tick(float dt)
{
	for (const auto& [key, state] : changed_keys) {
		for (const auto& callback : change_callbacks) {
			const key_state new_state = get_enum_from_state(key);
			if (new_state != key_state::nothing) {
				callback(key, new_state);
			}
		}
	}
	
	changed_keys.clear();
}

void
input::update_key(int16_t scan_code, bool state)
{
	changed_keys[scan_code] = state;
}

void
input::update_mouse_pos(ark_int_vec2 pos)
{
	delta_mouse_pos = pos - current_mouse_pos;
	current_mouse_pos = pos;
}

bool
input::is_key_pressed(int16_t scan_code)
{
	return keys_states[scan_code];
}

ark_int_vec2&
input::get_mouse_pos()
{
	return current_mouse_pos;
}

ark_int_vec2&
input::get_mouse_delta()
{
	return delta_mouse_pos;
}

void
input::subscribe_key_event(const on_key_change& input_callback)
{
	change_callbacks.insert(input_callback);
}

void
input::unsubscribe_key_event(const on_key_change& input_callback)
{
	change_callbacks.erase(input_callback);
}

/*
void
input::subscribe_input_event(event_type type, const event::callback& input_callback)
{
	ark_assert(input_name_map.contains(type), "map doesn't contain required event input type", return);
	//event::subscribe(input_name_map[type], input_callback);
}

void
input::unsubscribe_input_event(event_type type, const event::callback& input_callback)
{
	ark_assert(input_name_map.contains(type), "map doesn't contain required event input type", return);
///	event::unsubscribe(input_name_map[type], input_callback);
}

*/
