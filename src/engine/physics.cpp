#include "pch.h"

using namespace ark;

physics::world game_world;

void
physics::start()
{
	game_world.start();
}

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

ark_matrix
physics::get_body_position(physics_body* body)
{
	return game_world.get_body_position(body);
}

physics::physics_body*
physics::schedule_creation(body_parameters parameters)
{
	return game_world.schedule_creation(parameters);
}

void
physics::schedule_free(physics_body* body)
{
	game_world.schedule_free(body);
}

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const ark_float_vec2& point)
		: m_point(point) {}

	bool ReportFixture(b2Fixture* fixture) override
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	ark_float_vec2 m_point = {};
	b2Fixture* m_fixture = nullptr;
};

b2Body*
physics::hit_test(ark_float_vec2 pos)
{	
	// Make a small box.
	b2AABB aabb = {};
	ark_float_vec2 d = {};
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = pos - d;
	aabb.upperBound = pos + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(pos);
	game_world.get_world().QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		return callback.m_fixture->GetBody();
	}

	return nullptr;
}

void
physics::destroy_world()
{
	game_world.destroy_world();
}

marl::Event&
physics::get_physics_event()
{
	return game_world.physics_event;
}

b2World&
physics::get_world()
{
	return game_world.get_world();
}
