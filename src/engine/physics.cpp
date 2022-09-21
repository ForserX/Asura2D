#include "pch.h"

using namespace ark;

physics::world game_world;

void
physics::init()
{
	material::init();
	game_world.init();
}

void
physics::destroy()
{
	game_world.destroy();
	material::destroy();
}

void
physics::tick(float dt)
{
	game_world.tick(dt);
}

fmatrix
physics::get_body_position(b2Body* body)
{
	return game_world.get_body_position(body);
}

b2Body*
physics::create_static(b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat)
{
	return game_world.create_static(pos, shape, mat);
}

void
physics::destroy_world()
{
	game_world.destroy_world();
}

b2Body*
physics::create_dynamic(b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat)
{
	return game_world.create_dynamic(pos, shape, mat);
}

b2Body*
physics::create_dynamic_cricle(b2Vec2 pos, b2Vec2 shape, physics::material::material_type mat)
{
	return game_world.create_around(pos, shape, mat);
}
