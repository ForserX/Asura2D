#include "pch.h"

using namespace ark;

std::chrono::nanoseconds current_time;

void
engine::init(int argc, char** argv)
{
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
}

void
engine::tick()
{
	const auto new_time = std::chrono::steady_clock::now().time_since_epoch();
	const auto dt = static_cast<float>(static_cast<double>(new_time.count() - current_time.count()) / 1000000000.);
	current_time = new_time;
	
	game::tick(dt);
	render::tick(dt);
}
