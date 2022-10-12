#pragma once

namespace ark::window
{
	void init();
	void destroy();
	void tick();

	bool is_destroyed();

	void loop();
	void change_fullscreen();
	void change_resolution();
	void change_window_mode();
}
