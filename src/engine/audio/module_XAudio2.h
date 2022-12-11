#pragma once

namespace Asura::audio::xaudio2
{
	void Init();
	void Tick();
	void Destroy();

	void start(stl::string_view sound_src);
};
