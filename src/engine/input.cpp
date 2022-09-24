#include "pch.h"

using namespace ark;

float keys_states[1024] = {};

ark_int_vec2 current_mouse_pos = {};
ark_float_vec2 delta_mouse_pos = {};

std::unordered_map<int16_t, float> changed_keys;
stl_shit::function_set<input::on_key_change> key_change_callbacks;
stl_shit::function_set<input::on_input_change> input_change_callbacks;

input::key_state
get_enum_from_state(int16_t scan_code)
{
	const bool new_state = static_cast<bool>(changed_keys[scan_code]);
	if (new_state != static_cast<bool>(keys_states[scan_code])) {
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
		for (const auto& callback : key_change_callbacks) {
			const key_state new_state = get_enum_from_state(key);
			if (new_state != key_state::nothing) {
				callback(key, new_state);
			}
		}

		for (const auto& callback : input_change_callbacks) {
			callback(key, state);
		}

		if (key != SDL_SCANCODE_MOUSEWHEEL) {
			keys_states[key] = state;
		}
	}

	changed_keys.erase(SDL_SCANCODE_MOUSEWHEEL);
}

void
input::update_key(int16_t scan_code, float state)
{
	changed_keys[scan_code] = state;
}

void
input::update_mouse_pos(ark_int_vec2 pos)
{
	delta_mouse_pos = ark_float_vec2(pos.x, pos.y) - ark_float_vec2(current_mouse_pos.x, current_mouse_pos.y);
	current_mouse_pos = pos;
}

bool
input::is_key_pressed(int16_t scan_code)
{
	return static_cast<bool>(keys_states[scan_code]);
}

ark_int_vec2&
input::get_mouse_pos()
{
	return current_mouse_pos;
}

ark_float_vec2&
input::get_mouse_delta()
{
	return delta_mouse_pos;
}

const input::on_key_change&
input::subscribe_key_event(const on_key_change& input_callback)
{
	key_change_callbacks.emplace(input_callback);
	return input_callback;
}

void
input::unsubscribe_key_event(const on_key_change& input_callback)
{
	key_change_callbacks.erase(input_callback);
}

const input::on_input_change&
input::subscribe_input_event(const on_input_change& input_callback)
{
	input_change_callbacks.emplace(input_callback);
	return input_callback;
}

void
input::unsubscribe_input_event(const on_input_change& input_callback)
{
	input_change_callbacks.erase(input_callback);
}
