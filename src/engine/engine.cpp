#include "pch.h"

using namespace ark;

bool use_parallel = true;
marl::Scheduler engine_scheduler(marl::Scheduler::Config::allCores());

void
engine::start()
{
	physics::start();
}

void
engine::init(int argc, char** argv)
{
	OPTICK_THREAD("Main thread");
	engine_scheduler.bind();

	threads::init();
	filesystem::init();
	debug::init();
	render::init();
	game::init();
}

void
engine::destroy()
{
	game::destroy();
	render::destroy();
	debug::destroy();
	filesystem::destroy();
	threads::destroy();

	marl::Scheduler::unbind();
}

void
engine::tick()
{
	OPTICK_FRAME("Engine")
	OPTICK_EVENT("engine tick")
	static auto current_time = std::chrono::steady_clock::now().time_since_epoch();
	const auto new_time = std::chrono::steady_clock::now().time_since_epoch();
	auto dt = static_cast<float>(static_cast<double>(new_time.count() - current_time.count()) / 1000000000.);
	current_time = new_time;
	
	if (dt > 1) {
		dt = 0.06f;
	}

	game::tick(dt);
	render::tick(dt);
}
