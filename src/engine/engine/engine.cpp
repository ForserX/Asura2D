#include "pch.h"

using namespace Asura;

bool paused = false;
bool use_parallel = true;
static std::atomic_bool engine_ticking_now = {};

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
	Debug::msg("Initializing started:");

	Input::Init();
	Debug::msg("- Input: Done");

    console->Init();
    Debug::msg("- Console: : Done");
    
	Threads::Init();
    Debug::msg("- Threads: Done");
    
	Scheduler::Init();
    Debug::msg("- Scheduler: Done");
    
    ResourcesManager::Init();
    Debug::msg("- Resource Manager: Done");
    
	Event::Init();
    Debug::msg("- Event Manager: Done");
    
	Render::Init();
    Debug::msg("- Render: Done");
    
	game::Init();
    Debug::msg("Game Space: Done");

	Audio::Init();
	Debug::msg("Audio Manager: Done");
}

void engine::Destroy()
{
	game::Destroy();
	Render::Destroy();
	Input::Destroy();
	Event::Destroy();
    ResourcesManager::Destroy();
	Scheduler::Destroy();
	Threads::Destroy();
	Audio::Destroy();

    console.reset();
    
    Debug::Destroy();
    FileSystem::Destroy();
}

void engine::Tick()
{
	engine_ticking_now = true;
	
	OPTICK_FRAME("Engine");
	OPTICK_EVENT("Engine Tick");

	static auto CurrentTime = std::chrono::steady_clock::now().time_since_epoch();
	const auto& NewTime = std::chrono::steady_clock::now().time_since_epoch();

	float DeltaTime = float(NewTime.count() - CurrentTime.count());
	DeltaTime /= 1000000000.f;

	// Sync time frame
	CurrentTime = NewTime;
	
	if (DeltaTime > 1)
	{
		DeltaTime = 0.06f;
	}

	Input::Tick(DeltaTime);

	if (!paused) 
	{
		is_game_ticking = true;
		game::Tick(DeltaTime);
		is_game_ticking = false;
	}
	
	Render::Tick(DeltaTime);
	
	Audio::Tick();

	engine_ticking_now = false;
}

bool engine::IsTicking()
{
	return engine_ticking_now;
}