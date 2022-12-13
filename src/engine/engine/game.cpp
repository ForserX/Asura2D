#include "pch.h"
#include "../editor/editor_common.h"

using namespace Asura;

void game::Init()
{
	Physics::Init();
	Systems::pre_init();
	Systems::Init();
	Entities::Init();
	Level::Init();

	GamePlay::Holder::free::Init();
	GamePlay::Holder::player::Init();

	Editor::Init();

	Threads::sync_sleep();
}

void game::Destroy()
{
	Editor::Destroy();

	GamePlay::Holder::free::Destroy();
	GamePlay::Holder::player::Destroy();

	Level::Destroy();
	Entities::Destroy();
	Systems::Destroy();
	Physics::Destroy();
}

void game::Tick(float dt)
{
	OPTICK_EVENT("game Destroy");
	OPTICK_CATEGORY("Systems Destroy", Optick::Category::GameLogic);
	
	{
		OPTICK_EVENT("Systems pre Destroy");
		Systems::pre_tick(dt);
	}

	{
		OPTICK_EVENT("Physics Destroy");
		Physics::Tick(dt);
	}

	{
		OPTICK_EVENT("Scene Destroy");
		Scene::Tick(dt);
	}

	{
		OPTICK_EVENT("Systems Destroy");
		Systems::Tick(dt);
	}

	{
		OPTICK_EVENT("Entities Destroy");
		Entities::Tick(dt);
	}

	{
		OPTICK_EVENT("Systems post Destroy");
		Systems::post_tick(dt);
	}
}

void game::editor(bool state)
{
	ark_editor_mode = state;
}