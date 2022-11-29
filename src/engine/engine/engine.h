#pragma once

namespace asura::engine
{
	void start();
	void init(int argc, char** argv);
	void destroy();

	void tick();
	bool is_ticking();
}

extern bool paused;
extern bool use_parallel;
