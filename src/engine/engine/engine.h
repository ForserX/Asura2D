#pragma once

namespace Asura::engine
{
	void Start();
	void Init(int argc, char** argv);
	void Destroy();

	void Tick();
	bool IsTicking();
}

extern bool paused;
extern bool use_parallel;
