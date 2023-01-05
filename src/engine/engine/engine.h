#pragma once

namespace Asura::engine
{
	void Start();
	void Init();
	void Destroy();

	void Tick();
	bool IsTicking();
}

extern bool paused;
extern bool use_parallel;
