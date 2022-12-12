#include "pch.h"

using namespace Asura;

bool paused = false;
bool use_parallel = true;
std::atomic_bool engine_ticking_now = {};

void engine::start()
{
	Physics::Start();
}

void engine::Init(int argc, char** argv)
{
	OPTICK_THREAD("Main thread");
    
    console = std::make_unique<UI::Console>();
    
    FileSystem::Init();
    Debug::Init();

	input::Init();
	Debug::msg("initializing input system");

    console->Init();
    Debug::msg("base Systems inited. intializing other Systems");
    
	Threads::Init();
    Debug::msg("initializing Threads system");
    
	Scheduler::Init();
    Debug::msg("initializing Scheduler system");
    
    ResourcesManager::Init();
    Debug::msg("initializing resource system");
    
	Event::Init();
    Debug::msg("initializing event system");
    
	Render::Init();
    Debug::msg("initializing Render system");
    
	game::Init();
    Debug::msg("initializing game system");

	audio::Init();
	Debug::msg("initializing audio system");
}

void engine::Destroy()
{
	game::Destroy();
	Render::Destroy();
	input::Destroy();
	Event::Destroy();
    ResourcesManager::Destroy();
	Scheduler::Destroy();
	Threads::Destroy();
	audio::Destroy();

    console.reset();
    
    Debug::Destroy();
    FileSystem::Destroy();
}

void engine::Tick()
{
	engine_ticking_now = true;
	
	OPTICK_FRAME("Engine");
	OPTICK_EVENT("Engine Tick");

	static auto current_time = std::chrono::steady_clock::now().time_since_epoch();
	const auto new_time = std::chrono::steady_clock::now().time_since_epoch();
	auto dt = static_cast<float>(static_cast<double>(new_time.count() - current_time.count()) / 1000000000.);
	current_time = new_time;
	
	if (dt > 1) 
	{
		dt = 0.06f;
	}

	input::Tick(dt);

	if (!paused) 
	{
		is_game_ticking = true;
		game::Tick(dt);
		is_game_ticking = false;
	}
	
	Render::Tick(dt);
	
	audio::Tick();

	engine_ticking_now = false;
}

bool engine::IsTicking()
{
	return engine_ticking_now;
}
