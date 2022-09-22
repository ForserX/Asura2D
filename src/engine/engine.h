#pragma once

namespace ark::engine
{
	void start();
	void init(int argc, char** argv);
	void destroy();

	void tick();
}

extern bool use_parallel;
extern marl::Scheduler engine_scheduler;