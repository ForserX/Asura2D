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
	marl::mutex& mutex = physics::get_physics_mutex();

	//mutex.lock();
	systems::pre_tick(dt);

	physics::tick(dt);
		
	systems::tick(dt);
	systems::post_tick(dt);

	entities::tick(dt);
	//mutex.unlock();
}
