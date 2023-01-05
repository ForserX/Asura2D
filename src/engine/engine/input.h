#pragma once

namespace Asura::Input
{
	enum class MouseKey: int16_t
	{
		Left,
		Right,
		Center
	};

	enum class State : int16_t
	{
		Press,
		Release,
		Hold,
		Nothing
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

	using on_key_change = fu2::function<void(int16_t scan_code, State state) const>;
	using on_input_change = fu2::function<void(int16_t scan_code, float new_state) const>;
	
	int64_t Emplace(const on_key_change& input_callback);
	int64_t Emplace(const on_input_change& input_callback);
	void Erase(int64_t input_callback);
}
