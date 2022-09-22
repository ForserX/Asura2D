#include "pch.h"
using namespace ark;

void
game::init()
{
	physics::init();
	systems::pre_init();
	systems::init();
	entities::init();
}

void
game::destroy()
{
	entities::destroy();
	systems::destroy();
	physics::destroy();
}

void
game::tick(float dt)
{
	OPTICK_EVENT("game tick")
	OPTICK_CATEGORY("systems tick", Optick::Category::GameLogic)
	
	{
		OPTICK_EVENT("systems pre tick")
		systems::pre_tick(dt);
	}

	{
		OPTICK_EVENT("physics tick")
		physics::tick(dt);
	}

	{
		OPTICK_EVENT("systems tick")
		systems::tick(dt);
	}

	{
		OPTICK_EVENT("systems post tick")
		systems::post_tick(dt);
	}
	
	{
		OPTICK_EVENT("entities tick")
		entities::tick(dt);
	}
}
