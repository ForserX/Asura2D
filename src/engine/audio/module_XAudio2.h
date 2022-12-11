#pragma once

namespace asura::audio::xaudio2
{
	void init();
	void tick();
	void destroy();

	void start(stl::string_view sound_src);
};
