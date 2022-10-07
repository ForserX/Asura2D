#include "pch.h"

using namespace ark;

bool paused = false;
bool use_parallel = true;
std::atomic_bool engine_ticking_now = {};

void
engine::start()
{
	physics::start();
}

void
engine::init(int argc, char** argv)
{
	OPTICK_THREAD("Main thread");

	threads::init();
	scheduler::init();
	event::init();
	filesystem::init();
	debug::init();
	input::init();
	render::init();
	game::init();
}

void
engine::destroy()
{
	game::destroy();
	render::destroy();
	debug::destroy();
	input::destroy();
	filesystem::destroy();
	event::destroy();
	scheduler::destroy();
	threads::destroy();
}

void
engine::tick()
{
	engine_ticking_now = true;
	
	OPTICK_FRAME("Engine")
	OPTICK_EVENT("engine tick")
	static auto current_time = std::chrono::steady_clock::now().time_since_epoch();
	const auto new_time = std::chrono::steady_clock::now().time_since_epoch();
	auto dt = static_cast<float>(static_cast<double>(new_time.count() - current_time.count()) / 1000000000.);
	current_time = new_time;
	
	if (dt > 1) {
		dt = 0.06f;
	}

	input::tick(dt);
	if (!paused) {
		is_game_ticking = true;
		game::tick(dt);
		is_game_ticking = false;
	}
	
	render::tick(dt);
	
	engine_ticking_now = false;
}

bool
engine::is_ticking()
{
	return engine_ticking_now;
}
