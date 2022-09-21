#pragma once

namespace ark::input
{
	enum class mouse_key : int16_t
	{
		left_button,
		right_button,
		middle_button
	};

	enum class event_type
	{
		key_state_change,
		mouse_key_state_change
	};

	using on_key_state_changed = fu2::function<void(int16_t)>;
	using on_mouse_key_state_changed = fu2::function<void(mouse_key)>;
	using on_mouse_position_changed = fu2::function<void(ark_int_vec2)>;

	using on_key_pressed = fu2::function<void()>;
	using on_key_released = fu2::function<void()>;

	using on_event_triggered = fu2::function<void(const std::string_view&)>;
	 
	void init();
	void destroy();
	void tick(float dt);

	// event window listeners
	void update_key(int16_t scan_code, bool state);
	void update_mouse_key(mouse_key key, bool state);
	void update_mouse_pos(ark_int_vec2 pos);

	// state checkers
	bool is_key_pressed(int16_t scan_code);
	bool is_mouse_key_pressed(mouse_key key);
	ark_int_vec2 get_mouse_pos();
	ark_int_vec2 get_mouse_delta();

	// event creating
	void create_event(std::string_view name);
	
	// subscribers
	void subscribe_key_state_change(on_key_state_changed key_state_changed_delegate);
	void subscribe_mouse_key_state_change(on_mouse_key_state_changed mouse_key_state_changed_delegate);
	void subscribe_mouse_position_change(on_mouse_position_changed mouse_position_changed_delegate);
	
}
