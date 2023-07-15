#include "pch.h"

using namespace Asura;

bool paused = false;
bool use_parallel = true;
static std::atomic_bool engine_ticking_now = {};

void engine::Start()
{
	Physics::Start();
}

void engine::Init()
{
	OPTICK_THREAD("Main thread");
    
	ConsoleInstance = std::make_unique<UI::Console>();
    
    FileSystem::Init();
    Debug::Init();
	Debug::Msg("Initializing started:");

	Input::Init();
	Debug::Msg("- Input: Done");

    Debug::Msg("- Console: : Done");

	Threads::Init();
    Debug::Msg("- Threads: Done");
    
	Scheduler::Init();
    Debug::Msg("- Scheduler: Done");
    
    ResourcesManager::Init();
    Debug::Msg("- Resource Manager: Done");
    
	Render::Init();
	Render::Tick(0);
    Debug::Msg("- Render: Done");
    
	game::Init();
    Debug::Msg("Game Space: Done");

	Audio::Init();
	Debug::Msg("Audio Manager: Done");

	Console::MakeConsoleCommand<Console::CommandBoolean>("pause", &paused);

	Console::MakeConsoleCommand<Console::CommandBoolean>("window_fullscreen", &fullscreen_mode, Window::change_fullscreen);
	Console::MakeConsoleCommand<Console::CommandBoolean>("window_maximized", &window_maximized, Window::change_window_mode);
	Console::MakeConsoleCommand<Console::CommandNumber<int>>("window_height", &window_height, Window::change_resolution);
	Console::MakeConsoleCommand<Console::CommandNumber<int>>("window_width", &window_width, Window::change_resolution);

	ConsoleInstance->Init();
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

	ConsoleInstance.reset();
    
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