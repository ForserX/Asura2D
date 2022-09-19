#include "arkane.h"
#include "physical.h"

using ark::merry_boar;

merry_boar::merry_boar()
{
	b2Vec2 gravity(0.0f, 9.8f);
	world = std::make_unique<b2World>(gravity);
}

void merry_boar::create_ground(b2Vec2 base, b2Vec2 shape)
{
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(base.x, base.x);

	ground = std::make_unique<b2Body>(world->CreateBody(&groundBodyDef));

	b2PolygonShape groundBox;
	groundBox.SetAsBox(shape.x, shape.y);
	ground->CreateFixture(&groundBox, 0.0f);
}
