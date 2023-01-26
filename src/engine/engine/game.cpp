#include "pch.h"
#include "../editor/editor_common.h"

using namespace Asura;

void game::Init()
{
	Physics::Init();
	Systems::PreInit();
	Systems::Init();
	Entities::Init();
	Level::Init();

	GamePlay::Holder::Free::Init();
	GamePlay::Holder::Player::Init();
	GamePlay::Holder::PlayerFree::Init();

	Editor::Init();

	Threads::SyncCurrentThread();
}

void game::Destroy()
{
	Editor::Destroy();

	GamePlay::Holder::Free::Destroy();
	GamePlay::Holder::Player::Destroy();

	Level::Destroy();
	Entities::Destroy();
	Systems::Destroy();
	Physics::Destroy();
}

void game::Tick(float dt)
{
	OPTICK_EVENT("game Destroy");
#ifdef OPTICK_ENABLED
	OPTICK_CATEGORY("Systems Destroy", Optick::Category::GameLogic);
#endif
	{
		OPTICK_EVENT("Systems pre Destroy");
		Systems::BeforTick(dt);
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
		Systems::AfterTick(dt);
	}
}

void game::editor(bool state)
{
	is_editor_mode = state;
}