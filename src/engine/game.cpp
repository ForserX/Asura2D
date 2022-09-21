#include "pch.h"
using namespace ark;

void
game::init()
{
	physics::init();
	systems::pre_init();
	systems::init();
}

void
game::destroy()
{
	systems::destroy();
	physics::destroy();
}

void
game::tick(float dt)
{
	systems::pre_tick(dt);

	physics::tick(dt);
	systems::physics_tick(dt);
	
	systems::tick(dt);
	systems::post_tick(dt);
}
