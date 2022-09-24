﻿#pragma once

#define SDL_SCANCODE_MOUSE_LEFT SDL_SCANCODE_ENDCALL + SDL_BUTTON_LEFT
#define SDL_SCANCODE_MOUSE_MIDDLE SDL_SCANCODE_ENDCALL + SDL_BUTTON_MIDDLE
#define SDL_SCANCODE_MOUSE_RIGHT SDL_SCANCODE_ENDCALL + SDL_BUTTON_RIGHT
#define SDL_SCANCODE_MOUSE_X1 SDL_SCANCODE_ENDCALL + SDL_BUTTON_X1
#define SDL_SCANCODE_MOUSE_X2 SDL_SCANCODE_ENDCALL + SDL_BUTTON_X2

namespace ark::input
{
	enum class mouse_key : int16_t
	{
		left_button,
		right_button,
		middle_button
	};

	enum class key_state : int16_t
	{
		press,
		release,
		hold,
		nothing
	};
	
	void init();
	void destroy();
	void tick(float dt);

	// event window listeners
	void update_key(int16_t scan_code, bool state);
	void update_mouse_pos(ark_int_vec2 pos);

	// state checkers
	bool is_key_pressed(int16_t scan_code);
	ark_int_vec2& get_mouse_pos();
	ark_int_vec2& get_mouse_delta();

	using on_key_change = fu2::function<void(int16_t scan_code, key_state state) const>;
	void subscribe_key_event(const on_key_change& input_callback);
	void unsubscribe_key_event(const on_key_change& input_callback);
	
	// subscribers
	//void subscribe_input_event(event_type type, const event::callback& input_callback);
	//void unsubscribe_input_event(event_type type, const event::callback& input_callback);
	
}
