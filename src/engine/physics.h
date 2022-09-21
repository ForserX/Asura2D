#pragma once

namespace ark::physics
{
	void init();
	void tick(float dt);
	void destroy();

	world get_world();
	fmatrix get_body_position(b2Body* body);
	
	b2Body* create_ground(b2Vec2 pos, b2Vec2 shape);
	void destroy_world();
	b2Body* create_body(b2Vec2 pos, b2Vec2 shape);
}