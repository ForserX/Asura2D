#include "pch.h"

using namespace Asura;

bool paused = false;
bool use_parallel = true;
static std::atomic_bool engine_ticking_now = {};

void engine::Start()
{
	Physics::Start();
}

void engine::Init(int argc, char** argv)
{
	OPTICK_THREAD("Main thread");
    
    console = std::make_unique<UI::Console>();
    
    FileSystem::Init();
    Debug::Init();
	Debug::Msg("Initializing started:");

	Input::Init();
	Debug::Msg("- Input: Done");

    console->Init();
    Debug::Msg("- Console: : Done");
    
	Threads::Init();
    Debug::Msg("- Threads: Done");
    
	Scheduler::Init();
    Debug::Msg("- Scheduler: Done");
    
    ResourcesManager::Init();
    Debug::Msg("- Resource Manager: Done");
    
	Render::Init();
    Debug::Msg("- Render: Done");
    
	game::Init();
    Debug::Msg("Game Space: Done");

	Audio::Init();
	Debug::Msg("Audio Manager: Done");

#if 0
	// File stream tests
	Writer WData;
	WData.Push(32);
	WData.Push(false);
	WData.Push(-22ll);
	WData.Save("temp.bin");

	Reader RData("temp.bin");

	int IntTest = RData.Get<int>();
	bool BoolTest = RData.Get<bool>();
	long long LLTest = RData.Get<long long>();
#endif
}

void engine::Destroy()
{
	game::Destroy();
	Render::Destroy();
	Input::Destroy();
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