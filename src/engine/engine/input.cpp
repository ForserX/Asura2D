#include "pch.h"

using namespace Asura;

float keys_states[1024] = {};

Math::IVec2 previous_mouse_pos = {};
Math::IVec2 current_mouse_pos = {};
Math::FVec2 delta_mouse_pos = {};

stl::hash_map<int16_t, float> changed_keys;
stl::function_set<Input::on_key_change> key_change_callbacks;
stl::function_set<Input::on_input_change> input_change_callbacks;

Input::key_state get_enum_from_state(int16_t scan_code)
{
	const int new_state = static_cast<int>(changed_keys[scan_code]);
	if (new_state != static_cast<int>(keys_states[scan_code])) {
		return new_state ? Input::key_state::press : Input::key_state::release;
	}

	return new_state ? Input::key_state::hold : Input::key_state::nothing;
}

void Input::Init()
{
    changed_keys = {};
    key_change_callbacks = {};
    input_change_callbacks = {};
}

void Input::Destroy()
{
}

void Input::Tick(float dt)
{
	for (const auto& [key, state] : changed_keys) 
	{
		for (const auto& callback : key_change_callbacks)
		{
			key_state new_state = get_enum_from_state(key);
			if (new_state != key_state::nothing)
			{
				callback(key, new_state);
			}
		}

		for (const auto& callback : input_change_callbacks)
		{
			callback(key, state);
		}

		if (key != SDL_SCANCODE_MOUSEWHEEL)
		{
			keys_states[key] = state;
		}
	}

	delta_mouse_pos = Math::FVec2(current_mouse_pos.x, current_mouse_pos.y) - Math::FVec2(previous_mouse_pos.x, previous_mouse_pos.y);
	previous_mouse_pos = current_mouse_pos;
	changed_keys.erase(SDL_SCANCODE_MOUSEWHEEL);
}

bool Input::IsFocusedUI()
{
	return paused || show_console || ImGui::GetFocusID() != 0;
}

void Input::UpdateKey(int16_t scan_code, float state)
{
	changed_keys[scan_code] = state;
}

void Input::UpdateMousePos(Math::IVec2 pos)
{
	previous_mouse_pos = current_mouse_pos;
	current_mouse_pos = pos;
}

bool Input::IsKeyPressed(int16_t scan_code)
{
	return static_cast<bool>(keys_states[scan_code]);
}

Math::IVec2& Input::GetMousePos()
{
	return current_mouse_pos;
}

Math::FVec2& Input::GetMouseDelta()
{
	return delta_mouse_pos;
}

const Input::on_key_change& Input::SubscribeKeyEvent(const on_key_change& input_callback)
{
    key_change_callbacks.emplace(input_callback);
	return input_callback;
}

void Input::UnsubscribeKeyEvent(const on_key_change& input_callback)
{
	key_change_callbacks.erase(input_callback);
}

const Input::on_input_change& Input::SubscribeInputEvent(const on_input_change& input_callback)
{
	input_change_callbacks.emplace(input_callback);
	return input_callback;
}

void Input::UnsubscribeInputEvent(const on_input_change& input_callback)
{
	input_change_callbacks.erase(input_callback);
}
