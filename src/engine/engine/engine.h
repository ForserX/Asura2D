#pragma once

namespace Asura::engine
{
	void start();
	void Init(int argc, char** argv);
	void Destroy();

	void Tick();
	bool IsTicking();
}

extern bool paused;
extern bool use_parallel;
