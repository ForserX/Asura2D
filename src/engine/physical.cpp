#include "pch.h"

using ark::merry_boar;

merry_boar ark::physical;

constexpr int32 k_maxContactPoints = 2048;

class ark::CollisionLister : public b2ContactListener
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

	int32 m_pointCount;
	ContactPoint m_points[k_maxContactPoints];

	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
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

		b2WorldManifold worldManifold;
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

merry_boar::merry_boar()
{
	b2Vec2 gravity(0.0f, -9.8f);
	world = std::make_unique<b2World>(gravity);
	cl = std::make_unique<CollisionLister>();

	b2DestructionListener* m_destructionListener;
	world->SetContactListener(cl.get());

}

b2Body*
merry_boar::create_ground(b2Vec2 base, b2Vec2 shape)
{
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(base.x, base.x);

	ground = world->CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(shape.x, shape.y);
	ground->CreateFixture(&groundBox, 1.f);

	return ground;
}

ark::fmatrix
merry_boar::get_body_position(b2Body* body)
{
	fmatrix pos;

	b2AABB aabb;
	b2Transform t;
	t.SetIdentity();
	b2Fixture* fixture = body->GetFixtureList();
	while (fixture != NULL) {
		const b2Shape* shape = fixture->GetShape();
		const int childCount = shape->GetChildCount();
		for (int child = 0; child < childCount; ++child) {
			b2AABB shapeAABB;
			shape->ComputeAABB(&shapeAABB, t, child);
			shapeAABB.lowerBound = shapeAABB.lowerBound;
			shapeAABB.upperBound = shapeAABB.upperBound;
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

void
merry_boar::tick(float dt)
{
	static int32 velocityIterations = 6;
	static int32 positionIterations = 2;

	world->Step(dt, velocityIterations, positionIterations);
}

void
merry_boar::destroy_world()
{
	world.reset();
	ground = nullptr;
}

b2Body*
merry_boar::create_body(b2Vec2 pos, b2Vec2 shape)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(pos.x, pos.y);
	b2Body* body = world->CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(shape.x, shape.y);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 0.5f;
	fixtureDef.friction = 0.4f;
	fixtureDef.restitution = 0.f;

	body->CreateFixture(&fixtureDef);

	return body;
}
