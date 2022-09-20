#pragma once

namespace ark::window
{
	void init();
	void destroy();
	void tick();

	void loop();
	void change_fullscreen();
	void change_resolution();
}
