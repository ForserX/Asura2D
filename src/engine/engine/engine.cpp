#include "pch.h"

using namespace ark;

bool paused = false;
bool use_parallel = true;
std::atomic_bool engine_ticking_now = {};
extern std::unique_ptr<ui::console> console;

void
engine::start()
{
	physics::start();
}

void
engine::init(int argc, char** argv)
{
	OPTICK_THREAD("Main thread");
    
    console = std::make_unique<ui::console>();
    
    filesystem::init();
    debug::init();

	input::init();
	debug::msg("initializing input system");

    console->init();
    debug::msg("base systems inited. intializing other systems");
    
	threads::init();
    debug::msg("initializing threads system");
    
	scheduler::init();
    debug::msg("initializing scheduler system");
    
    resources::init();
    debug::msg("initializing resource system");
    
	event::init();
    debug::msg("initializing event system");
    
	render::init();
    debug::msg("initializing render system");
    
	game::init();
    debug::msg("initializing game system");

	audio::init();
	debug::msg("initializing audio system");
}

void
engine::destroy()
{
	game::destroy();
	render::destroy();
	input::destroy();
	event::destroy();
    resources::destroy();
	scheduler::destroy();
	threads::destroy();
	audio::destroy();

    console.reset();
    
    debug::destroy();
    filesystem::destroy();
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
	
	audio::tick();

	engine_ticking_now = false;
}

bool
engine::is_ticking()
{
	return engine_ticking_now;
}
