#include "pch.h"

using namespace ark;

physics::world game_world;

void
physics::init()
{
	game_world.init();
}

void
physics::destroy()
{
	game_world.destroy();
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
physics::create_ground(b2Vec2 pos, b2Vec2 shape)
{
	return game_world.create_ground(pos, shape);
}

void
physics::destroy_world()
{
	game_world.destroy_world();
}

b2Body*
physics::create_body(b2Vec2 pos, b2Vec2 shape)
{
	return game_world.create_body(pos, shape);
}
