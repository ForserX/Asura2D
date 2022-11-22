#pragma once

namespace ark::audio::openal
{
	void init();
	void tick();
	void destroy();

	void start(stl::string_view sound_src);
};
