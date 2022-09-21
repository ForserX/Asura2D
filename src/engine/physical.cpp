#include "pch.h"

using ark::merry_boar;

merry_boar ark::physical;

merry_boar::merry_boar()
{
	b2Vec2 gravity(0.0f, -9.8f);
	world = std::make_unique<b2World>(gravity);
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
	float timeStep = 1.0f / 60.0f;
	static int32 velocityIterations = 6;
	static int32 positionIterations = 2;

	world->Step(timeStep, velocityIterations, positionIterations);
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
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.5f;
	fixtureDef.restitution = 0.f;

	body->CreateFixture(&fixtureDef);

	return body;
}
