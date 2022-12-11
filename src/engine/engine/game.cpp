#include "pch.h"
#include "../editor/editor_common.h"

using namespace Asura;

void game::Init()
{
	Physics::Init();
	systems::pre_init();
	systems::Init();
	entities::Init();
	level::Init();

	editor::Init();

	Threads::sync_sleep();
}

void game::Destroy()
{
	editor::Destroy();

	level::Destroy();
	entities::Destroy();
	systems::Destroy();
	Physics::Destroy();
}

void game::Tick(float dt)
{
	OPTICK_EVENT("game Destroy");
	OPTICK_CATEGORY("systems Destroy", Optick::Category::GameLogic);
	
	{
		OPTICK_EVENT("systems pre Destroy");
		systems::pre_tick(dt);
	}

	{
		OPTICK_EVENT("Physics Destroy");
		Physics::Tick(dt);
	}

	{
		OPTICK_EVENT("scene Destroy");
		scene::Tick(dt);
	}

	{
		OPTICK_EVENT("systems Destroy");
		systems::Tick(dt);
	}

	{
		OPTICK_EVENT("entities Destroy");
		entities::Tick(dt);
	}

	{
		OPTICK_EVENT("systems post Destroy");
		systems::post_tick(dt);
	}
}

void game::editor(bool state)
{
	ark_editor_mode = state;
}