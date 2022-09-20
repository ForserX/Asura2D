#include "pch.h"

using ark::merry_boar;

merry_boar ark::physical;

merry_boar::merry_boar()
{
	b2Vec2 gravity(0.0f, -9.8f);
	world = std::make_unique<b2World>(gravity);
}

void
merry_boar::create_ground(b2Vec2 base, b2Vec2 shape)
{
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(base.x, base.x);

	ground = world->CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(shape.x, shape.y);
	ground->CreateFixture(&groundBox, 0.0f);
}

void
merry_boar::tick(float dt)
{
	float timeStep = 1.0f / dt;
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
	fixtureDef.friction = 0.3f;

	body->CreateFixture(&fixtureDef);

	return body;
}
