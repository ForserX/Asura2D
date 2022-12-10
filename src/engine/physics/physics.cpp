#include "pch.h"

using namespace asura;

physics::world game_world = {};
physics::physics_body* ground_base = nullptr;

void physics::start()
{
	game_world.start();
}

void physics::init()
{
	material::init();
	game_world.init();
}

void physics::destroy()
{
	game_world.destroy();
	material::destroy();

	// Auto deleted into World
	ground_base = nullptr;
}

void physics::tick(float dt)
{
	game_world.tick(dt);
}

math::frect physics::get_body_rect(const physics_body* body)
{
	return game_world.get_body_rect(body);
}

physics::physics_body* physics::schedule_creation(body_parameters parameters)
{
	return game_world.schedule_creation(parameters);
}

physics::physics_joint* asura::physics::schedule_creation(joint_data&& parameters)
{
	return game_world.schedule_creation(std::move(parameters));
}

physics::physics_body* physics::get_ground()
{
	if (ground_base == nullptr)
	{
		ground_base = new physics_body(game_world.get_ground());
	}

	return ground_base;
}

void physics::schedule_free(physics_body* body)
{
	game_world.schedule_free(body);
}

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const math::fvec2& point)
		: m_point(point) {}

	bool ReportFixture(b2Fixture* fixture) override
	{
		const b2Body* body = fixture->GetBody();
        if (fixture->TestPoint({m_point.x, m_point.y})) 
		{
			m_fixture = fixture;
			return false;
		}
		
		return true;
	}

    math::fvec2 m_point = {};
	b2Fixture* m_fixture = nullptr;
};

physics::physics_body* physics::hit_test(math::fvec2 pos)
{	
	// Make a small box.
	b2AABB aabb = {};
	b2Vec2 d = {};
    b2Vec2 phys_pos = {pos.x, pos.y};
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = phys_pos - d;
	aabb.upperBound = phys_pos + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(pos);
	game_world.get_world().QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		const b2Body* phys_body = callback.m_fixture->GetBody();
		const auto entity = entities::get_entity_from_body(phys_body);
		const auto phys_comp = entities::try_get<entities::physics_body_component>(entity);
		if (phys_comp != nullptr) 
		{
			return phys_comp->body;
		}
	}

	return nullptr;
}

void physics::destroy_world()
{
	game_world.destroy_world();
}

b2World& physics::get_world()
{
	return game_world.get_world();
}