#include "pch.h"

using namespace ark;

bool keys_states[4096] = {};
bool mouse_keys_states[12] = {};
ark_int_vec2 current_mouse_pos = {};
ark_int_vec2 delta_mouse_pos = {};

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
}

void
input::update_key(int16_t scan_code, bool state)
{
	keys_states[scan_code] = state;
}

void
input::update_mouse_key(mouse_key key, bool state)
{
	mouse_keys_states[static_cast<int16_t>(key)] = state;
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

bool
input::is_mouse_key_pressed(mouse_key key)
{
	return mouse_keys_states[static_cast<int16_t>(key)];
}

ark_int_vec2
input::get_mouse_pos()
{
	return current_mouse_pos;
}

ark_int_vec2
input::get_mouse_delta()
{
	return delta_mouse_pos;
}
