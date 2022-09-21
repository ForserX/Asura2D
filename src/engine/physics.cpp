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

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

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

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};

b2Body*
physics::hit_test(ImVec2 pos)
{
	b2Vec2 reintrp = *(b2Vec2*)&pos;
	// Make a small box.
	b2AABB aabb;
	b2Vec2 d;
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = reintrp - d;
	aabb.upperBound = reintrp + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(reintrp);
	game_world.get_world().QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		float frequencyHz = 5.0f;
		float dampingRatio = 0.7f;

		return callback.m_fixture->GetBody();
	}

	return nullptr;
}

void
physics::destroy_world()
{
	game_world.destroy_world();
}

b2World&
physics::get_world()
{
	return game_world.get_world();
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
