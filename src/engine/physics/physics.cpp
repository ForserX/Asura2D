#include "pch.h"

using namespace Asura;

Physics::PhysicsWorld game_world = {};
Physics::PhysicsBody* ground_base = nullptr;

void Physics::Start()
{
	game_world.Start();
}

void Physics::Init()
{
	Physics::Material::Init();
	game_world.Init();
}

void Physics::Destroy()
{
	game_world.Destroy();
	Physics::Material::Destroy();

	// Auto deleted into World
	ground_base = nullptr;
}

void Physics::Tick(float dt)
{
	game_world.Tick(dt);
}

Math::FRect Physics::GetBodyRect(const PhysicsBody* body)
{
	return game_world.GetBodyRect(body);
}

Physics::PhysicsBody* Physics::SafeCreation(body_parameters parameters)
{
	return game_world.SafeCreation(parameters);
}

Physics::PhysicsJoint* Asura::Physics::SafeCreation(joint_data&& parameters)
{
	return game_world.SafeCreation(std::move(parameters));
}

Physics::PhysicsBody* Physics::GetGround()
{
	if (ground_base == nullptr)
	{
		ground_base = new PhysicsBody(game_world.GetGround());
	}

	return ground_base;
}

void Physics::SafeFree(PhysicsBody* body)
{
	game_world.SafeFree(body);
}

class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const Math::FVec2& point)
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

    Math::FVec2 m_point = {};
	b2Fixture* m_fixture = nullptr;
};

Physics::PhysicsBody* Physics::HitTest(Math::FVec2 pos)
{	
	if (Input::IsFocusedUI())
	{
		// FX: Why the extra costs if we're stuck in the UI 
		return nullptr;
	}

	// Make a small box.
	b2AABB aabb = {};
	b2Vec2 d = {};
    b2Vec2 phys_pos = {pos.x, pos.y};
	d.Set(0.001f, 0.001f);
	aabb.lowerBound = phys_pos - d;
	aabb.upperBound = phys_pos + d;

	// Query the world for overlapping shapes.
	QueryCallback callback(pos);
	game_world.GetWorld().QueryAABB(&callback, aabb);

	if (callback.m_fixture)
	{
		const b2Body* phys_body = callback.m_fixture->GetBody();
		const auto entity = Entities::GetEntityByBbody(phys_body);
		const auto phys_comp = Entities::TryGet<Entities::physics_body_component>(entity);
		if (phys_comp != nullptr) 
		{
			return phys_comp->body;
		}
	}

	return nullptr;
}

void Physics::DestroyWorld()
{
	game_world.DestroyWorld();
}

b2World& Physics::GetWorld()
{
	return game_world.GetWorld();
}