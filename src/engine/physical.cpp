#include "arkane.h"
#include "physical.h"

using ark::merry_boar;

merry_boar::merry_boar() : ground(nullptr)
{
	b2Vec2 gravity(0.0f, -9.8f);
	world = std::make_unique<b2World>(gravity);
}

void merry_boar::create_ground(b2Vec2 base, b2Vec2 shape)
{
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(base.x, base.x);

	ground = world->CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(shape.x, shape.y);
	ground->CreateFixture(&groundBox, 0.0f);
}

void ark::merry_boar::destroy_world()
{
	world.reset();
}

b2Body* ark::merry_boar::create_boody(b2Vec2 pos, b2Vec2 shape)
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
