#pragma once

#define SDL_SCANCODE_MOUSE_LEFT SDL_SCANCODE_ENDCALL + SDL_BUTTON_LEFT
#define SDL_SCANCODE_MOUSE_MIDDLE SDL_SCANCODE_ENDCALL + SDL_BUTTON_MIDDLE
#define SDL_SCANCODE_MOUSE_RIGHT SDL_SCANCODE_ENDCALL + SDL_BUTTON_RIGHT
#define SDL_SCANCODE_MOUSE_X1 SDL_SCANCODE_ENDCALL + SDL_BUTTON_X1
#define SDL_SCANCODE_MOUSE_X2 SDL_SCANCODE_ENDCALL + SDL_BUTTON_X2
#define SDL_SCANCODE_MOUSEWHEEL SDL_SCANCODE_ENDCALL + SDL_SCANCODE_MOUSE_X2 + 1

namespace Asura::Input
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
	
	void Init();
	void Destroy();
	void Tick(float dt);

	bool IsFocusedUI();

	// event window listeners
	void UpdateKey(int16_t scan_code, float state);
	void UpdateMousePos(Math::IVec2 pos);

	// state checkers
	bool IsKeyPressed(int16_t scan_code);
	Math::IVec2& GetMousePos();
	Math::FVec2& GetMouseDelta();

	using on_key_change = fu2::function<void(int16_t scan_code, key_state state) const>;
	using on_input_change = fu2::function<void(int16_t scan_code, float new_state) const>;
	
	const on_key_change& SubscribeKeyEvent(const on_key_change& input_callback);
	void UnsubscribeKeyEvent(const on_key_change& input_callback);
	
	const on_input_change& SubscribeInputEvent(const on_input_change& input_callback);
	void UnsubscribeInputEvent(const on_input_change& input_callback);
}
