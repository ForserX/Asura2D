#include "pch.h"

using ark::physics::world;

constexpr int32 k_maxContactPoints = 2048;

class ark::CollisionLister final : public b2ContactListener
{
	struct ContactPoint
	{
		b2Fixture* fixtureA;
		b2Fixture* fixtureB;
		b2Vec2 normal;
		b2Vec2 position;
		b2PointState state;
		float normalImpulse;
		float tangentImpulse;
		float separation;
	};

	int32 m_pointCount = 0;
	ContactPoint m_points[k_maxContactPoints] = {};

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold) override
	{
		const b2Manifold* manifold = contact->GetManifold();

		if (manifold->pointCount == 0)
		{
			return;
		}

		b2Fixture* fixtureA = contact->GetFixtureA();
		b2Fixture* fixtureB = contact->GetFixtureB();

		b2PointState state1[b2_maxManifoldPoints], state2[b2_maxManifoldPoints];
		b2GetPointStates(state1, state2, oldManifold, manifold);

		b2WorldManifold worldManifold = {};
		contact->GetWorldManifold(&worldManifold);

		for (int32 i = 0; i < manifold->pointCount && m_pointCount < k_maxContactPoints; ++i)
		{
			ContactPoint* cp = m_points + m_pointCount;
			cp->fixtureA = fixtureA;
			cp->fixtureB = fixtureB;
			cp->position = worldManifold.points[i];
			cp->normal = worldManifold.normal;
			cp->state = state2[i];
			cp->normalImpulse = manifold->points[i].normalImpulse;
			cp->tangentImpulse = manifold->points[i].tangentImpulse;
			cp->separation = worldManifold.separations[i];
			++m_pointCount;
		}
	}
};

world::world()
{
}

world::~world()
{
}

void
world::init()
{
	b2Vec2 gravity(0.0f, -9.8f);
	world_holder = std::make_unique<b2World>(gravity);
	cl = std::make_unique<CollisionLister>();
	world_holder->SetContactListener(cl.get());
}

void
world::destroy()
{
	destroy_world();
}

ark::fmatrix
world::get_body_position(b2Body* body)
{
	fmatrix pos = {};
	b2AABB aabb = {};
	b2Transform t = {};
	
	t.SetIdentity();
	b2Fixture* fixture = body->GetFixtureList();
	while (fixture != nullptr) {
		const b2Shape* shape = fixture->GetShape();
		const int childCount = shape->GetChildCount();
		for (int child = 0; child < childCount; ++child) {
			b2AABB shapeAABB = {};
			shape->ComputeAABB(&shapeAABB, t, child);
			aabb.Combine(shapeAABB);
		}
		
		fixture = fixture->GetNext();
	}

	pos.x = body->GetPosition().x;
	pos.y = body->GetPosition().y;
	pos.w = aabb.upperBound.x;
	pos.h = aabb.upperBound.y;

	return pos;
}

constexpr float phys_tps = 30.f;
float phys_accum = 0.f;


void
world::tick(float dt) const
{
	static int32 velocityIterations = 6;
	static int32 positionIterations = 2;
	
	phys_accum += dt;
		world_holder->Step(1.f / phys_tps, velocityIterations, positionIterations);
		phys_accum = 0.f;

	world_holder->ClearForces();
}

b2World& 
world::get_world() const
{
	return *world_holder.get();
}

void
world::destroy_world()
{
	world_holder.reset();
}

b2Body*
world::create_around(b2Vec2 pos, b2Vec2 size, material::material_type mat) const
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(pos.x, pos.y);
	b2Body* body = world_holder->CreateBody(&bodyDef);
	
	b2MassData mass_data;
	mass_data.center = { size.x / 2, size.y / 2 };
	mass_data.mass = 30;

	body->SetMassData(&mass_data);

	b2CircleShape circle;
	circle.m_p.Set(size.x, size.y);
	circle.m_radius = size.x / 2;

	b2FixtureDef fixtureDef;
	const material::material_data& mdata = material::get(mat);

	fixtureDef.shape = &circle;
	fixtureDef.density = mdata.density;
	fixtureDef.friction = mdata.friction;
	fixtureDef.restitution = mdata.restitution;
	fixtureDef.isSensor = mdata.ignore_collision;

	body->CreateFixture(&fixtureDef);

	return body;
}

b2Body*
world::create_static(b2Vec2 base, b2Vec2 size, material::material_type mat) const
{
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(base.x, base.y);

	b2Body* ground = world_holder->CreateBody(&groundBodyDef);

	const material::material_data& mdata = material::get(mat);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(size.x, size.y);
	ground->CreateFixture(&groundBox, mdata.density);

	return ground;
}

b2Body*
world::create_dynamic(b2Vec2 pos, b2Vec2 size, material::material_type mat) const
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(pos.x, pos.y);
	b2Body* body = world_holder->CreateBody(&bodyDef);

	b2MassData mass_data;
	mass_data.center = { size.x / 2, size.y / 2 };
	mass_data.mass = 30;

	body->SetMassData(&mass_data);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(size.x, size.y);

	b2FixtureDef fixtureDef;
	const material::material_data& mdata = material::get(mat);

	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = mdata.density;
	fixtureDef.friction = mdata.friction;
	fixtureDef.restitution = mdata.restitution;
	fixtureDef.isSensor = mdata.ignore_collision;

	body->CreateFixture(&fixtureDef);

	return body;
}
