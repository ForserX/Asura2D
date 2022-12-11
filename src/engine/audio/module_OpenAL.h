#pragma once

namespace Asura::audio::openal
{
	void Init();
	void Tick();
	void Destroy();

	void start(stl::string_view sound_src);
};
