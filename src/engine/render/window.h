#pragma once

namespace Asura::window
{
	void Init();
	void Tick();
	void Destroy();

	bool IsDestroyed();

	void loop();
	void change_fullscreen();
	void change_resolution();
	void change_window_mode();
}
